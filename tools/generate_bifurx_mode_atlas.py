"""Insert deterministic, self-contained SVG mode diagrams into the Bifurx manual."""

from __future__ import annotations

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


# Keep the response and routing in separate vertical bands.
DIAGRAM_CSS = """.mode-diagram { display:block; width:230px; height:158px; max-width:none; background:#0a111b; border:1px solid #253342; border-radius:8px; }
.mode-diagram .axis { fill:none; stroke:#344353; stroke-width:0.8; }
.mode-diagram .focus-a,.mode-diagram .focus-b { fill:none; stroke-width:0.8; stroke-dasharray:2 4; stroke-opacity:0.5; }
.mode-diagram .focus-a { stroke:#72c5ce; }
.mode-diagram .focus-b { stroke:#b4a1db; }
.mode-diagram .focus-label-a,.mode-diagram .focus-label-b { font:600 10px ui-monospace,monospace; }
.mode-diagram .focus-label-a { fill:#72c5ce; }
.mode-diagram .focus-label-b { fill:#b4a1db; }
.mode-diagram .response { fill:none; stroke:var(--gold); stroke-width:2.2; stroke-linecap:round; stroke-linejoin:round; }
.mode-diagram .flow { fill:#12202e; stroke:#536b7a; stroke-width:1; font:600 10px ui-monospace,monospace; }
.mode-diagram .flow path { fill:none; }
.mode-diagram .flow .stage-a { stroke:#72c5ce; stroke-opacity:0.75; }
.mode-diagram .flow .stage-b { stroke:#b4a1db; stroke-opacity:0.75; }
.mode-diagram .flow text { fill:#d5e0e9; stroke:none; }
@media print { .mode-diagram { background:#fff; border-color:#9caaba; } .mode-diagram .flow { fill:#e7f3f6; } .mode-diagram .response { stroke:#805c13; } .mode-diagram .flow text,.mode-diagram .focus-label-a,.mode-diagram .focus-label-b { fill:#182a3b; } }
"""


def diagram(number: int, name: str, topology: str, a: str, b: str) -> str:
    # Five octaves, with the foci 1.7 octaves apart and equal side margins.
    focus_offset = 0.85
    log_min, log_max = -2.5, 2.5
    plot_left, plot_right = 10.0, 220.0
    points = []
    for index in range(301):
        x = plot_left + index * (plot_right - plot_left) / 300
        frequency = 2 ** (log_min + index * (log_max - log_min) / 300)
        ra = response(a, frequency, 2 ** -focus_offset)
        rb = response(b, frequency, 2 ** focus_offset)
        amplitude = ra * rb if topology == "cascade" else (ra + rb) / 2
        db = 20 * math.log10(max(abs(amplitude), 1e-5))
        # Clip the drawing, not the amplitude: tails leave the plot naturally.
        y = 47 - db * 1.6
        points.append(f"{x:.2f},{y:.2f}")
    curve = " ".join(points)
    ax = plot_left + (-focus_offset - log_min) / (log_max - log_min) * (plot_right - plot_left)
    bx = plot_left + (focus_offset - log_min) / (log_max - log_min) * (plot_right - plot_left)
    if topology == "cascade":
        flow = f'<text x="14" y="138">IN</text><path d="M32 134H46 M104 134H126 M184 134H198"/><rect class="stage-a" x="46" y="123" width="58" height="22" rx="5"/><rect class="stage-b" x="126" y="123" width="58" height="22" rx="5"/><text x="75" y="138" text-anchor="middle">A {a}</text><text x="155" y="138" text-anchor="middle">B {b}</text><text x="201" y="138">OUT</text>'
    else:
        flow = f'<text x="14" y="141">IN</text><path d="M32 137H60 M60 123V151 M60 123H86 M60 151H86 M144 123H170 M144 151H170 M170 123V151 M170 137H198"/><rect class="stage-a" x="86" y="112" width="58" height="22" rx="5"/><rect class="stage-b" x="86" y="140" width="58" height="22" rx="5"/><text x="115" y="127" text-anchor="middle">A {a}</text><text x="115" y="155" text-anchor="middle">B {b}</text><text x="201" y="141">OUT</text>'
    return (
        f'<svg class="mode-diagram" viewBox="0 0 230 158" role="img" '
        f'aria-label="{name}: {topology} response and signal route" xmlns="http://www.w3.org/2000/svg">'
        f'<title>{name}: {topology} response</title>'
        f'<defs><clipPath id="bifurx-mode-{number}-plot"><rect x="10" y="18" width="210" height="78"/></clipPath></defs>'
        '<path class="axis" d="M10 47H220"/>'
        f'<path class="focus-a" d="M{ax:.1f} 20V96"/><path class="focus-b" d="M{bx:.1f} 20V96"/>'
        f'<text class="focus-label-a" x="{ax:.1f}" y="13" text-anchor="middle">A</text>'
        f'<text class="focus-label-b" x="{bx:.1f}" y="13" text-anchor="middle">B</text>'
        f'<g clip-path="url(#bifurx-mode-{number}-plot)"><polyline class="response" points="{curve}"/></g>'
        f'<g class="flow" transform="translate(0 -8)">{flow}</g></svg>'
    )


def display_diagram() -> str:
    return '<svg class="mode-diagram" viewBox="0 0 230 158" role="img" aria-label="Display Only: direct pass-through" xmlns="http://www.w3.org/2000/svg"><title>Display Only: direct pass-through</title><path class="axis" d="M10 47H220"/><path class="response" d="M10 47H220"/><g class="flow" transform="translate(0 -8)"><text x="14" y="138">IN</text><path d="M32 134H198"/><text x="201" y="138">OUT</text></g></svg>'


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
        # Refresh the styles too, so existing atlases receive layout changes.
        html, count = re.subn(
            r"^\.mode-diagram \{.*?^@media print \{ \.mode-diagram .*?\n",
            lambda _: DIAGRAM_CSS, html, count=1, flags=re.MULTILINE | re.DOTALL,
        )
        if count != 1:
            raise SystemExit("Could not find the mode diagram stylesheet")
        target.write_text(html, encoding="utf-8")
        return
    html = html.replace(
        ".modes td:nth-child(4) {",
        DIAGRAM_CSS + ".modes td:nth-child(5) {",
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
