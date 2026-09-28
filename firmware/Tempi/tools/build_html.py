#!/usr/bin/env python3
"""Build the optional self-contained reading edition. Requires mistune >= 3.
The supplied index.html is already built; this is not needed for firmware recovery.
"""
from pathlib import Path
import html,re
import mistune
ROOT=Path(__file__).resolve().parents[1]
sections=[('overview','Engineering report','REPORT.md'),('transport','WAV transport','docs/TRANSPORT.md'),
          ('io','Controls and I/O','docs/IO_MAP.md'),('storage','Memory and states','docs/MEMORY_AND_STATES.md'),
          ('algorithms','Recovered algorithms','docs/ALGORITHMS.md'),('bus','Select Bus protocol','docs/SELECT_BUS.md'),
          ('validation','Validation and limits','docs/VALIDATION_AND_LIMITS.md'),('next','Porting and next work','docs/PORTING_AND_NEXT_STEPS.md'),
          ('sources','Sources and provenance','references/SOURCES.md')]
md=mistune.create_markdown(escape=True,plugins=['table','url'])
css='''
:root{--ink:#1c2838;--muted:#536579;--paper:#fff;--bg:#edf2f5;--line:#d6e0e7;--accent:#006d77;--nav:#122836}
*{box-sizing:border-box}html{scroll-behavior:smooth}body{margin:0;background:var(--bg);color:var(--ink);font:16px/1.66 -apple-system,BlinkMacSystemFont,"Segoe UI",Arial,sans-serif}
.sidebar{position:fixed;inset:0 auto 0 0;width:258px;overflow:auto;background:var(--nav);color:#eaf3f7;padding:28px 22px}.brand{font-size:12px;letter-spacing:.15em;text-transform:uppercase;color:#91c9cf}.sidebar h2{font-size:27px;line-height:1.2;margin:12px 0 26px}.sidebar a{display:block;padding:8px 0;color:#dbe9ee;text-decoration:none;font-size:14px}.sidebar a:hover{color:#90e2e7}.sidebar .small{font-size:12px;color:#a4b5c2;border-top:1px solid #3a4c5a;margin-top:24px;padding-top:18px}
main{margin-left:258px;padding:36px 4vw 70px;max-width:1510px}.hero{background:var(--nav);color:white;border-radius:12px;padding:38px 42px;margin-bottom:28px}.eyebrow{text-transform:uppercase;letter-spacing:.15em;font-size:12px;color:#9bdddf}.hero h1{font-size:40px;line-height:1.14;margin:12px 0 18px}.hero p{max-width:880px;color:#d8e4ec}.chips{display:flex;flex-wrap:wrap;gap:10px;margin:24px 0 0}.chip{padding:8px 13px;background:#25414e;border:1px solid #42606d;border-radius:6px;font-size:13px}.chip strong{color:#a6f1da}.notice{background:#fff3d9;border-left:4px solid #c28218;padding:16px 22px;border-radius:4px;margin:22px 0;color:#4f4027}
article{background:var(--paper);border:1px solid var(--line);border-radius:10px;padding:32px 42px;margin:24px 0;scroll-margin-top:20px}article h1{font-size:30px;line-height:1.25;color:#124853;margin-top:0}h2{font-size:23px;line-height:1.3;margin:36px 0 16px;padding-top:14px;border-top:1px solid var(--line)}h3{font-size:19px;line-height:1.35;margin-top:26px}p{margin:13px 0}a{color:#006d77;overflow-wrap:anywhere}strong{font-weight:650}code{font:13px/1.55 ui-monospace,SFMono-Regular,Consolas,monospace;background:#eff4f7;padding:2px 4px;border-radius:3px;overflow-wrap:anywhere}pre{background:#132b38;color:#e9f2f8;border-radius:7px;padding:18px 20px;overflow:auto;line-height:1.5}pre code{background:none;padding:0;color:inherit;white-space:pre;overflow-wrap:normal}table{width:100%;border-collapse:collapse;margin:20px 0;font-size:13px;line-height:1.5;table-layout:auto}th{background:#e9f1f4;color:#153e4b;font-weight:650;text-align:left}td,th{padding:10px 12px;border:1px solid var(--line);vertical-align:top;overflow-wrap:anywhere}tbody tr:nth-child(even){background:#f8fafb}blockquote{border-left:4px solid var(--accent);padding:1px 20px;margin-left:0;color:var(--muted)}li{margin:8px 0}.download{display:flex;flex-wrap:wrap;gap:10px}.download a{padding:9px 14px;border:1px solid #bed6dd;border-radius:5px;text-decoration:none;background:#f7fbfc;font-size:14px}.section-number{color:#758b9c;font-size:12px;text-transform:uppercase;letter-spacing:.14em}.footer{font-size:12px;color:var(--muted)}
@media(max-width:1050px){.sidebar{position:static;width:auto;padding:18px 22px}.sidebar h2{margin:8px 0;font-size:23px}.sidebar nav{display:flex;flex-wrap:wrap;gap:0 20px}.sidebar .small{display:none}main{margin-left:0;padding:18px}.hero{padding:26px}.hero h1{font-size:32px}article{padding:22px}table{font-size:12px}td,th{padding:8px}}
@media print{body{background:white;font-size:10pt}.sidebar,.download,.chips{display:none}main{margin:0;padding:0;max-width:none}.hero{background:white!important;color:#132b38;padding:0;border-radius:0}.hero p,.eyebrow{color:#3f5263}.hero h1{font-size:28pt}article{border:0;border-radius:0;padding:0;margin:25px 0;break-before:page}h1,h2,h3{break-after:avoid}pre{white-space:pre-wrap;background:#f1f5f7;color:#172b3a;break-inside:avoid}pre code{white-space:pre-wrap}tr{break-inside:avoid}thead{display:table-header-group}table{font-size:8pt}.notice{background:#faf4e7}a{color:inherit}}
'''
nav=''.join(f'<a href="#{id}">{html.escape(title)}</a>' for id,title,_ in sections)
parts=[]
for n,(id,title,path) in enumerate(sections,1):
    content=md((ROOT/path).read_text())
    parts.append(f'<article id="{id}"><div class="section-number">{n:02d} / {html.escape(title)}</div>{content}</article>')
page=f'''<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Tempi 71 — Firmware Recovery Dossier</title><style>{css}</style></head><body>
<aside class="sidebar"><div class="brand">Firmware engineering</div><h2>TEMPI 71<br>Recovery dossier</h2><nav>{nav}</nav><p class="small">27 September 2026<br>Source: user-supplied WAV<br>Program addresses: bytes<br>Analyst labels ≠ original symbols</p></aside>
<main><header class="hero"><div class="eyebrow">Exact payload recovery · PIC18 analysis</div><h1>Inside Tempi’s firmware</h1><p>The WAV yields readable Intel HEX and a PIC18 application—not ARM. This dossier connects the recovered bytes to clock math, state storage, physical I/O, and Select Bus behavior.</p><div class="chips"><span class="chip"><strong>2,323</strong> checksums pass</span><span class="chip"><strong>37,110</strong> recovered bytes</span><span class="chip"><strong>16,422</strong> decoded instructions</span><span class="chip"><strong>Exact</strong> PCM round trip</span></div></header>
<div class="notice"><strong>Evidence boundary:</strong> a recovered application is not a complete hardware image. The resident bootloader, actual EEPROM contents, physical clock frequency, and analog circuitry are not recovered. Synthetic and padded files are explicitly marked.</div>
<div class="download"><a href="firmware/tempi71_recovered.hex">Recovered Intel HEX</a><a href="analysis/disassembly_reachable.asm">Addressed disassembly</a><a href="analysis/functions.csv">Function lookup</a><a href="analysis/io_map.csv">GPIO map</a><a href="analysis/validation.json">Validation results</a><a href="README.md">Package guide</a></div>
{''.join(parts)}<p class="footer">Independent analysis of the supplied file. Not an official Make Noise specification. All extracted bytes, source hashes, test limitations, and inferred labels are documented in the package.</p></main></body></html>'''
(ROOT/'index.html').write_text(page)
print(f'Wrote {ROOT/"index.html"}: {len(page.encode())} bytes')
