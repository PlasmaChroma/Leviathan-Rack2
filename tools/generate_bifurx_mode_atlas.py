"""Insert deterministic, self-contained SVG mode diagrams into the Bifurx manual."""

from __future__ import annotations

import cmath
import math
import re
from pathlib import Path


MODES = [
    ("Low + Low", "cascade", "LP", "LP"),
    ("Low + Band", "parallel", "LP", "BP"),
    ("Notch + Low", "cascade", "Notch", "LP"),
    ("Notch + Notch", "cascade", "Notch", "Notch"),
    ("Low + High", "parallel", "LP", "HP"),
    ("Band + Band", "parallel", "BP", "BP"),
    ("High + Low", "cascade", "HP", "LP"),
    ("High + Notch", "cascade", "HP", "Notch"),
    ("Band + High", "parallel", "BP", "HP"),
    ("High + High", "cascade", "HP", "HP"),
]


def response(kind: str, frequency: float, focus: float) -> complex:
    s = 1j * frequency / focus
    q = 5.5
    denominator = 1 + s / q + s * s
    numerator = {"LP": 1, "BP": s, "HP": s * s, "Notch": 1 + s * s}[kind]
    return numerator / denominator


def diagram(number: int, name: str, topology: str, a: str, b: str) -> str:
    # Show five octaves in a compact plot with the foci 1.7 octaves apart.
    # Equal side margins keep the response centered above the signal route.
    focus_offset = 0.85
    log_min, log_max = -2.5, 2.5
    plot_left, plot_right = 10.0, 220.0
    points = []
    for index in range(101):
        x = plot_left + index * (plot_right - plot_left) / 100
        frequency = 2 ** (log_min + index * (log_max - log_min) / 100)
        ra = response(a, frequency, 2 ** -focus_offset)
        rb = response(b, frequency, 2 ** focus_offset)
        amplitude = ra * rb if topology == "cascade" else (ra + rb) / 2
        db = 20 * math.log10(max(abs(amplitude), 1e-5))
        y = 55 - max(-30, min(18, db)) * 1.15
        points.append(f"{x:.1f},{y:.1f}")
    curve = " ".join(points)
    ax = plot_left + (-focus_offset - log_min) / (log_max - log_min) * (plot_right - plot_left)
    bx = plot_left + (focus_offset - log_min) / (log_max - log_min) * (plot_right - plot_left)
    if topology == "cascade":
        flow = f'<text x="22" y="105">IN</text><path d="M42 101H60 M106 101H124 M170 101H188"/><rect x="60" y="90" width="46" height="22" rx="5"/><rect x="124" y="90" width="46" height="22" rx="5"/><text x="83" y="105" text-anchor="middle">A {a}</text><text x="147" y="105" text-anchor="middle">B {b}</text><text x="196" y="105">OUT</text>'
    else:
        flow = f'<text x="22" y="105">IN</text><path d="M42 101H58V91H77 M58 101V112H97 M123 91H165V112 M143 112H165 M165 101H188"/><rect x="77" y="80" width="46" height="22" rx="5"/><rect x="97" y="102" width="46" height="22" rx="5"/><text x="100" y="95" text-anchor="middle">A {a}</text><text x="120" y="117" text-anchor="middle">B {b}</text><text x="196" y="105">OUT</text>'
    return (
        f'<svg class="mode-diagram" viewBox="0 0 230 128" role="img" '
        f'aria-label="{name}: {topology} response and signal route" xmlns="http://www.w3.org/2000/svg">'
        f'<title>{name}: {topology} response</title>'
        '<path class="axis" d="M10 55H220"/>'
        f'<path class="focus-a" d="M{ax:.1f} 12V84"/><path class="focus-b" d="M{bx:.1f} 12V84"/>'
        f'<text class="focus-label-a" x="{ax:.1f}" y="11" text-anchor="middle">A</text>'
        f'<text class="focus-label-b" x="{bx:.1f}" y="11" text-anchor="middle">B</text>'
        f'<polyline class="response" points="{curve}"/>'
        f'<g class="flow">{flow}</g></svg>'
    )


def display_diagram() -> str:
    return '<svg class="mode-diagram" viewBox="0 0 230 128" role="img" aria-label="Display Only: direct pass-through" xmlns="http://www.w3.org/2000/svg"><title>Display Only: direct pass-through</title><path class="axis" d="M10 55H220"/><path class="response" d="M10 55H220"/><g class="flow"><text x="22" y="105">IN</text><path d="M42 101h146"/><text x="196" y="105">OUT</text></g></svg>'


def main() -> None:
    target = Path(__file__).resolve().parents[2] / "Leviathan-Pages/manuals/Bifurx_User_Manual.html"
    html = target.read_text(encoding="utf-8")
    if 'class="mode-diagram"' in html:
        replacements = iter([diagram(number, *mode) for number, mode in enumerate(MODES, 1)] + [display_diagram()])
        found = 0

        def refresh(match: re.Match[str]) -> str:
            nonlocal found
            found += 1
            return next(replacements) if found <= len(MODES) + 1 else match.group(0)

        html = re.sub(r'<svg class="mode-diagram".*?</svg>', refresh, html)
        if found != 11:
            raise SystemExit(f"Expected 11 existing diagrams, found {found}")
        html = html.replace("font:600 8px ui-monospace,monospace", "font:600 10px ui-monospace,monospace")
        target.write_text(html, encoding="utf-8")
        return
    html = html.replace(
        ".modes td:nth-child(4) {",
        ".mode-diagram { display:block; width:230px; height:128px; max-width:none; background:#0a111b; border:1px solid #314154; border-radius:8px; }\n"
        ".mode-diagram .axis { stroke:#425065; stroke-width:1; }\n"
        ".mode-diagram .focus-a,.mode-diagram .focus-b { stroke:#8193a5; stroke-width:1; stroke-dasharray:3 3; }\n"
        ".mode-diagram .focus-label-a,.mode-diagram .focus-label-b { fill:#d9e6f2; font:700 10px ui-monospace,monospace; }\n"
        ".mode-diagram .response { fill:none; stroke:var(--gold); stroke-width:2.4; stroke-linecap:round; stroke-linejoin:round; }\n"
        ".mode-diagram .flow { fill:#122438; stroke:#5ccbd5; stroke-width:1.2; font:600 10px ui-monospace,monospace; }\n"
        ".mode-diagram .flow text { fill:#e4f0f8; stroke:none; }\n"
        "@media print { .mode-diagram { background:#fff; border-color:#9caaba; } .mode-diagram .flow { fill:#e7f3f6; } .mode-diagram .flow text,.mode-diagram .focus-label-a,.mode-diagram .focus-label-b { fill:#182a3b; } }\n"
        ".modes td:nth-child(5) {",
        1,
    )
    html = html.replace("<th>Routing</th><th>Musical character</th>", "<th>Routing</th><th>Shape + flow</th><th>Musical character</th>", 1)
    for number, (name, topology, a, b) in enumerate(MODES, 1):
        pattern = rf'(<tr><td>{number}</td><td>{re.escape(name)}</td><td>.*?</td><td>.*?</td>)(<td>)'
        html, count = re.subn(pattern, lambda m: m.group(1) + "<td>" + diagram(number, name, topology, a, b) + "</td>" + m.group(2), html, count=1)
        if count != 1:
            raise SystemExit(f"Could not find mode {number}: {name}")
    html = html.replace('<tr><td>11</td><td>Display Only</td><td>Pass-through</td><td>IN -&gt; OUT</td><td>', '<tr><td>11</td><td>Display Only</td><td>Pass-through</td><td>IN -&gt; OUT</td><td>' + display_diagram() + '</td><td>', 1)
    target.write_text(html, encoding="utf-8")


if __name__ == "__main__":
    main()
