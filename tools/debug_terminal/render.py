import shutil
import sys
import time

try:
    from rich.console import Console, Group
    from rich.live import Live
    from rich.table import Table
    from rich.text import Text
except ImportError:
    Console = None
    Group = None
    Live = None
    Table = None
    Text = None


MODULE_COLUMNS = {
    "TDScope": (
        ("ui_ms", "UI ms"),
        ("rows", "Rows"),
        ("density_pct", "Density%"),
        ("zoom", "Zoom"),
        ("thickness", "Thickness"),
        ("misses", "Misses"),
    ),
    "TemporalDeck": (
        ("ui_ms", "UI ms"),
        ("scope_preview_us", "Scope us"),
        ("scope_stride", "Stride"),
        ("scope_metric_valid", "Scope OK"),
    ),
}


def _format_metric(value):
    if value is None:
        return "-"
    if isinstance(value, float):
        return "%.2f" % value
    return str(value)


def _module_columns(module_name):
    return MODULE_COLUMNS.get(module_name, tuple())


def _group_rows_by_module(snapshot):
    grouped = {}
    for row in snapshot["rows"]:
        grouped.setdefault(row["module"], []).append(row)
    return grouped


def _module_title(module_name, row_count):
    if Text is None:
        return "%s (%d)" % (module_name, row_count)
    title = Text()
    title.append(module_name, style="bold")
    title.append(" (%d)" % row_count, style="dim")
    return title


def build_module_table(module_name, rows):
    table = Table(title=_module_title(module_name, len(rows)))
    table.add_column("Instance", no_wrap=True)
    table.add_column("Stream", no_wrap=True)
    for _, label in _module_columns(module_name):
        table.add_column(label, justify="right", no_wrap=True)
    table.add_column("Age", justify="right", no_wrap=True)

    for row in rows:
        metrics = row["data"]
        cells = [row["instance"], row["stream"]]
        for key, _ in _module_columns(module_name):
            cells.append(_format_metric(metrics.get(key)))
        cells.append("%.2fs" % row["age_sec"])
        table.add_row(*cells)

    return table


def build_table(snapshot, host, port):
    renderables = []
    if Text is None:
        renderables.append("Debug Terminal %s:%d" % (host, port))
        renderables.append(
            "clients=%d  rows=%d  events=%d  parse_errors=%d  eps=%.1f"
            % (
                snapshot["client_count"],
                len(snapshot["rows"]),
                snapshot["events_total"],
                snapshot["parse_errors"],
                snapshot["events_per_sec"],
            )
        )
    else:
        title = Text()
        title.append("Debug Terminal ", style="bold")
        title.append("%s:%d" % (host, port), style="cyan")
        renderables.append(title)

        status = Text()
        status.append("clients=%d" % snapshot["client_count"], style="green")
        status.append("  ")
        status.append("rows=%d" % len(snapshot["rows"]), style="white")
        status.append("  ")
        status.append("events=%d" % snapshot["events_total"], style="white")
        status.append("  ")
        status.append("parse_errors=%d" % snapshot["parse_errors"], style="yellow" if snapshot["parse_errors"] else "dim")
        status.append("  ")
        status.append("eps=%.1f" % snapshot["events_per_sec"], style="magenta")
        renderables.append(status)

    grouped = _group_rows_by_module(snapshot)
    for module_name in sorted(grouped.keys()):
        renderables.append(build_module_table(module_name, grouped[module_name]))
    return Group(*renderables)


def _truncate(text, width):
    text = str(text)
    if width <= 0:
        return ""
    if len(text) <= width:
        return text
    if width == 1:
        return text[:1]
    if width <= 3:
        return text[:width]
    return text[: width - 3] + "..."


def _plain_module_lines(module_name, rows):
    columns = _module_columns(module_name)
    header = ["Instance", "Stream"] + [label for _, label in columns] + ["Age"]
    table_rows = []
    for row in rows:
        data = row["data"]
        values = [row["instance"], row["stream"]]
        for key, _ in columns:
            values.append(_format_metric(data.get(key)))
        values.append("%.2fs" % row["age_sec"])
        table_rows.append(values)

    widths = [len(label) for label in header]
    for values in table_rows:
        for i, value in enumerate(values):
            widths[i] = max(widths[i], len(str(value)))

    max_widths = [14, 10] + [10 for _ in columns] + [8]
    widths = [min(widths[i], max_widths[i]) for i in range(len(widths))]

    align_right = [False, False] + [True for _ in columns] + [True]

    def format_row(values):
      cells = []
      for i, value in enumerate(values):
          cell = _truncate(value, widths[i])
          if align_right[i]:
              cells.append(cell.rjust(widths[i]))
          else:
              cells.append(cell.ljust(widths[i]))
      return "  ".join(cells).rstrip()

    lines = ["%s (%d)" % (module_name, len(rows)), format_row(header)]
    for values in table_rows:
        lines.append(format_row(values))
    return lines


def build_plain_text(snapshot, host, port):
    lines = [
        "Debug Terminal %s:%d" % (host, port),
        "clients=%d  rows=%d  events=%d  parse_errors=%d  eps=%.1f"
        % (
            snapshot["client_count"],
            len(snapshot["rows"]),
            snapshot["events_total"],
            snapshot["parse_errors"],
            snapshot["events_per_sec"],
        ),
        "",
    ]

    grouped = _group_rows_by_module(snapshot)
    for module_name in sorted(grouped.keys()):
        lines.extend(_plain_module_lines(module_name, grouped[module_name]))
        lines.append("")

    width = shutil.get_terminal_size((120, 40)).columns
    return "\n".join(_truncate(line, width) for line in lines).rstrip() + "\n"


def run_live_renderer(state, host, port, refresh_hz, stop_event):
    if Console is None or Live is None or Table is None:
        return run_plain_renderer(state, host, port, refresh_hz, stop_event)

    console = Console()
    interval_sec = 1.0 / max(1.0, float(refresh_hz))
    with Live(console=console, refresh_per_second=max(1.0, float(refresh_hz))) as live:
        while not stop_event.is_set():
            live.update(build_table(state.snapshot(), host, port))
            stop_event.wait(interval_sec)


def run_plain_renderer(state, host, port, refresh_hz, stop_event):
    interval_sec = 1.0 / max(1.0, float(refresh_hz))
    use_ansi = sys.stdout.isatty()
    if use_ansi:
        sys.stdout.write("\x1b[?25l\x1b[2J")
        sys.stdout.flush()
    try:
        while not stop_event.is_set():
            frame = build_plain_text(state.snapshot(), host, port)
            if use_ansi:
                sys.stdout.write("\x1b[H\x1b[J")
                sys.stdout.write(frame)
                sys.stdout.flush()
            else:
                sys.stdout.write(frame)
                sys.stdout.flush()
            stop_event.wait(interval_sec)
    finally:
        if use_ansi:
            sys.stdout.write("\x1b[?25h")
            sys.stdout.flush()
