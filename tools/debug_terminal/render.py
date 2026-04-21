import os
import time

try:
    from rich.console import Console
    from rich.live import Live
    from rich.table import Table
except ImportError:
    Console = None
    Live = None
    Table = None


METRIC_COLUMNS = (
    ("ui_ms", "UI ms"),
    ("rows", "Rows"),
    ("density_pct", "Density%"),
    ("zoom", "Zoom"),
    ("thickness", "Thickness"),
    ("misses", "Misses"),
)


def _format_metric(value):
    if value is None:
        return "-"
    if isinstance(value, float):
        return "%.2f" % value
    return str(value)


def build_table(snapshot, host, port):
    table = Table(title="Debug Terminal %s:%d" % (host, port))
    table.add_column("Module", no_wrap=True)
    table.add_column("Instance", no_wrap=True)
    table.add_column("Stream", no_wrap=True)
    for _, label in METRIC_COLUMNS:
        table.add_column(label, justify="right", no_wrap=True)
    table.add_column("Age", justify="right", no_wrap=True)

    for row in snapshot["rows"]:
        metrics = row["data"]
        cells = [row["module"], row["instance"], row["stream"]]
        for key, _ in METRIC_COLUMNS:
            cells.append(_format_metric(metrics.get(key)))
        cells.append("%.2fs" % row["age_sec"])
        table.add_row(*cells)

    table.caption = (
        "clients=%d  rows=%d  events=%d  parse_errors=%d  eps=%.1f"
        % (
            snapshot["client_count"],
            len(snapshot["rows"]),
            snapshot["events_total"],
            snapshot["parse_errors"],
            snapshot["events_per_sec"],
        )
    )
    return table


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
        header = ["Module", "Instance", "Stream", "UI", "Rows", "Density", "Zoom", "Thick", "Miss", "Age"]
        print("{:<12} {:<14} {:<8} {:>8} {:>8} {:>8} {:>8} {:>8} {:>8} {:>8}".format(*header))
        for row in snapshot["rows"]:
            data = row["data"]
            print(
                "{:<12} {:<14} {:<8} {:>8} {:>8} {:>8} {:>8} {:>8} {:>8} {:>8}".format(
                    row["module"][:12],
                    row["instance"][:14],
                    row["stream"][:8],
                    _format_metric(data.get("ui_ms")),
                    _format_metric(data.get("rows")),
                    _format_metric(data.get("density_pct")),
                    _format_metric(data.get("zoom")),
                    _format_metric(data.get("thickness")),
                    _format_metric(data.get("misses")),
                    "%.2fs" % row["age_sec"],
                )
            )
        stop_event.wait(interval_sec)
