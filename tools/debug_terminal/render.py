import os
import time

try:
    from rich.console import Console
    from rich.live import Live
    from rich.table import Table
    from rich.text import Text
except ImportError:
    Console = None
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
    outer = Table(title="Debug Terminal %s:%d" % (host, port), show_header=False, box=None, pad_edge=False)
    outer.add_column("Tables")

    grouped = _group_rows_by_module(snapshot)
    for module_name in sorted(grouped.keys()):
        outer.add_row(build_module_table(module_name, grouped[module_name]))

    outer.caption = (
        "clients=%d  rows=%d  events=%d  parse_errors=%d  eps=%.1f"
        % (
            snapshot["client_count"],
            len(snapshot["rows"]),
            snapshot["events_total"],
            snapshot["parse_errors"],
            snapshot["events_per_sec"],
        )
    )
    return outer


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
    while not stop_event.is_set():
        snapshot = state.snapshot()
        os.system("clear")
        print("Debug Terminal %s:%d" % (host, port))
        print(
            "clients=%d rows=%d events=%d parse_errors=%d eps=%.1f"
            % (
                snapshot["client_count"],
                len(snapshot["rows"]),
                snapshot["events_total"],
                snapshot["parse_errors"],
                snapshot["events_per_sec"],
            )
        )
        print("")

        grouped = _group_rows_by_module(snapshot)
        for module_name in sorted(grouped.keys()):
            rows = grouped[module_name]
            columns = _module_columns(module_name)
            print("%s (%d)" % (module_name, len(rows)))
            header = ["Instance", "Stream"] + [label for _, label in columns] + ["Age"]
            formats = ["{:<14}", "{:<8}"] + ["{:>10}" for _ in columns] + ["{:>8}"]
            row_fmt = " ".join(formats)
            print(row_fmt.format(*header))
            for row in rows:
                data = row["data"]
                values = [row["instance"][:14], row["stream"][:8]]
                for key, _ in columns:
                    values.append(_format_metric(data.get(key)))
                values.append("%.2fs" % row["age_sec"])
                print(row_fmt.format(*values))
            print("")

        stop_event.wait(interval_sec)
