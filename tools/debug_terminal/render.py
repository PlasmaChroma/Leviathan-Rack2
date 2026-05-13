import shutil
import sys
import time
import threading

try:
    import termios
    import tty
    import select
except ImportError:
    termios = None
    tty = None
    select = None

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
        ("publish_seq", "Publish"),
        ("draw_seq", "Draw"),
        ("draw_calls", "Calls"),
    ),
    "TemporalDeck": (
        ("ui_ms", "UI ms"),
        ("scope_preview_us", "Scope us"),
        ("scope_stride", "Stride"),
        ("scope_metric_valid", "Scope OK"),
    ),
    "Bifurx": (
        ("ui_ms", "UI ms"),
        ("filter", "Filt"),
        ("opengl", "GL"),
        ("audio_us", "Audio us"),
        ("curve_prep_us", "Curve us"),
        ("overlay_prep_us", "Overlay us"),
    ),
    "Wyrm": (
        ("ui_ms", "UI ms"),
        ("ed_us", "Ed us"),
        ("sand_up_us", "SUp us"),
        ("sand_dr_us", "SDr us"),
        ("audio_us", "Aud us"),
        ("ch", "Ch"),
        ("pts", "Pts"),
        ("rocks", "Rock"),
        ("sand", "Sand"),
        ("body", "Body"),
        ("fm", "FM"),
        ("fold", "Fold"),
        ("slith", "Slith"),
        ("lfo", "LFO"),
        ("wt", "WT"),
    ),
}

KEY_UP = "up"
KEY_DOWN = "down"
KEY_TOGGLE = "toggle"
KEY_COLLAPSE_ALL = "collapse_all"
KEY_EXPAND_ALL = "expand_all"
KEY_QUIT = "quit"


class ModuleViewState(object):
    def __init__(self):
        self._lock = threading.Lock()
        self._module_order = []
        self._selected_module = None
        self._collapsed = set()

    def update_modules(self, module_names):
        module_names = list(module_names)
        with self._lock:
            self._module_order = module_names
            if not self._module_order:
                self._selected_module = None
                return
            if self._selected_module not in self._module_order:
                self._selected_module = self._module_order[0]

    def handle_key(self, key):
        with self._lock:
            if not self._module_order:
                return
            if self._selected_module not in self._module_order:
                self._selected_module = self._module_order[0]
            idx = self._module_order.index(self._selected_module)
            if key == KEY_UP:
                idx = (idx - 1) % len(self._module_order)
                self._selected_module = self._module_order[idx]
            elif key == KEY_DOWN:
                idx = (idx + 1) % len(self._module_order)
                self._selected_module = self._module_order[idx]
            elif key == KEY_TOGGLE:
                module_name = self._selected_module
                if module_name in self._collapsed:
                    self._collapsed.remove(module_name)
                else:
                    self._collapsed.add(module_name)
            elif key == KEY_COLLAPSE_ALL:
                self._collapsed = set(self._module_order)
            elif key == KEY_EXPAND_ALL:
                self._collapsed.clear()

    def snapshot(self):
        with self._lock:
            return self._selected_module, set(self._collapsed)


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


def _module_title(module_name, row_count, selected=False, collapsed=False):
    marker = "[+]" if collapsed else "[-]"
    if Text is None:
        selected_prefix = ">" if selected else " "
        return "%s %s %s (%d)" % (selected_prefix, marker, module_name, row_count)
    title = Text()
    title.append(marker + " ", style="dim")
    if selected:
        title.append("▶ ", style="yellow")
    title.append(module_name, style="bold")
    title.append(" (%d)" % row_count, style="dim")
    return title


def build_module_table(module_name, rows, selected=False, collapsed=False):
    if collapsed:
        if Text is None:
            return _module_title(module_name, len(rows), selected=selected, collapsed=True)
        summary = Text()
        summary.append(_module_title(module_name, len(rows), selected=selected, collapsed=True))
        summary.append("  ")
        summary.append("collapsed", style="dim")
        return summary

    table = Table(title=_module_title(module_name, len(rows), selected=selected, collapsed=False))
    table.add_column("ID", no_wrap=True)
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


def build_table(snapshot, host, port, view_state=None):
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
    module_names = sorted(grouped.keys())
    if view_state is not None:
        view_state.update_modules(module_names)
        selected_module, collapsed_modules = view_state.snapshot()
    else:
        selected_module = None
        collapsed_modules = set()

    for module_name in module_names:
        renderables.append(
            build_module_table(
                module_name,
                grouped[module_name],
                selected=(module_name == selected_module),
                collapsed=(module_name in collapsed_modules),
            )
        )
    if Text is None:
        renderables.append("Controls: j/k move  space/enter toggle  c collapse-all  e expand-all  q quit")
    else:
        controls = Text()
        controls.append("Controls: ", style="dim")
        controls.append("j/k", style="cyan")
        controls.append(" move  ", style="dim")
        controls.append("space/enter", style="cyan")
        controls.append(" toggle  ", style="dim")
        controls.append("c", style="cyan")
        controls.append(" collapse-all  ", style="dim")
        controls.append("e", style="cyan")
        controls.append(" expand-all  ", style="dim")
        controls.append("q", style="cyan")
        controls.append(" quit", style="dim")
        renderables.append(controls)
    renderables.append("")
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
    header = ["ID", "Stream"] + [label for _, label in columns] + ["Age"]
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
    lines.append("")

    width = shutil.get_terminal_size((120, 40)).columns
    return "\n".join(_truncate(line, width) for line in lines).rstrip() + "\n"


def _read_key_nonblocking():
    if not sys.stdin.isatty():
        return None
    if select is not None:
        ready, _, _ = select.select([sys.stdin], [], [], 0.0)
        if not ready:
            return None
    ch = sys.stdin.read(1)
    if ch == "\x1b":
        # Arrow keys arrive as ESC [ A/B
        if select is not None:
            ready, _, _ = select.select([sys.stdin], [], [], 0.0)
            if not ready:
                return None
        ch2 = sys.stdin.read(1)
        if ch2 != "[":
            return None
        if select is not None:
            ready, _, _ = select.select([sys.stdin], [], [], 0.0)
            if not ready:
                return None
        ch3 = sys.stdin.read(1)
        if ch3 == "A":
            return KEY_UP
        if ch3 == "B":
            return KEY_DOWN
        return None
    if ch in ("j", "J"):
        return KEY_UP
    if ch in ("k", "K"):
        return KEY_DOWN
    if ch in (" ", "\n", "\r"):
        return KEY_TOGGLE
    if ch in ("c", "C"):
        return KEY_COLLAPSE_ALL
    if ch in ("e", "E"):
        return KEY_EXPAND_ALL
    if ch in ("q", "Q"):
        return KEY_QUIT
    return None


def _keyboard_loop(stop_event, view_state):
    if termios is None or tty is None or not sys.stdin.isatty():
        return
    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)
    try:
        tty.setcbreak(fd)
        while not stop_event.is_set():
            key = _read_key_nonblocking()
            if key is None:
                stop_event.wait(0.02)
                continue
            if key == KEY_QUIT:
                stop_event.set()
                break
            view_state.handle_key(key)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)


def run_live_renderer(state, host, port, refresh_hz, stop_event):
    if Console is None or Live is None or Table is None:
        return run_plain_renderer(state, host, port, refresh_hz, stop_event)

    console = Console()
    interval_sec = 1.0 / max(1.0, float(refresh_hz))
    view_state = ModuleViewState()
    key_thread = threading.Thread(target=_keyboard_loop, args=(stop_event, view_state), daemon=True)
    key_thread.start()
    with Live(
        console=console,
        refresh_per_second=max(1.0, float(refresh_hz)),
        screen=True,
        transient=False,
        vertical_overflow="crop",
    ) as live:
        while not stop_event.is_set():
            live.update(build_table(state.snapshot(), host, port, view_state=view_state))
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
