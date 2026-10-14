#!/usr/bin/env python3
"""Build a self-contained HTML reading edition; requires mistune for rendering."""
from pathlib import Path
import html,re
import mistune
ROOT=Path(__file__).resolve().parents[1]
parts=[('report','Main analysis','REPORT.md'),('dsp','DSP equations','DSP_SPEC.md'),('arrays','Array format','ARRAY_FORMAT.md'),('controls','Controls & I/O','CONTROL_IO_MAP.md'),('codex','Codex handoff','CODEX_HANDOFF.md'),('tools','Tools & reproduction','TOOLS_AND_REPRODUCTION.md'),('sources','Sources & provenance','SOURCES.md')]
md=mistune.create_markdown(escape=True,plugins=['table','url'])
nav=''.join(f'<a href="#{slug}">{html.escape(title)}</a>' for slug,title,_ in parts)
body=[];combined=[]
for slug,title,name in parts:
    text=(ROOT/'docs'/name).read_text();combined.append(text)
    section=md(text)
    section=re.sub(r'\[S([1-6])\]',lambda m:f'<a class="source" href="#sources">[S{m.group(1)}]</a>',section)
    body.append(f'<section id="{slug}"><div class="section-kicker">{html.escape(title)}</div>{section}</section>')
css='''
:root{--ink:#18313a;--muted:#546973;--accent:#13777c;--line:#d4e0e2;--wash:#edf4f4;--nav:#102b34}
*{box-sizing:border-box}html{scroll-behavior:smooth}body{margin:0;background:#f4f7f7;color:var(--ink);font:16px/1.65 system-ui,-apple-system,Segoe UI,sans-serif}
header{background:var(--nav);color:#fff;padding:48px max(24px,calc((100vw - 1120px)/2));border-bottom:5px solid #3bb5af}header .eyebrow{font-size:12px;letter-spacing:.19em;text-transform:uppercase;color:#98d9d6}header h1{font-size:clamp(32px,5vw,52px);line-height:1.12;margin:12px 0 16px}header p{max-width:870px;color:#d2e4e8;margin:0}header .meta{margin-top:20px;font-size:13px;color:#b7d2d7}
nav{position:sticky;top:0;z-index:3;background:#fff;border-bottom:1px solid var(--line);display:flex;gap:8px;flex-wrap:wrap;justify-content:center;padding:12px 18px;box-shadow:0 3px 8px #102b3409}nav a{font-size:13px;font-weight:650;text-decoration:none;color:var(--accent);padding:6px 9px;border-radius:5px}nav a:hover{background:var(--wash)}
main{max-width:1160px;margin:28px auto 80px;padding:0 20px}section{background:#fff;border:1px solid var(--line);border-radius:10px;padding:34px 46px;margin-bottom:28px;scroll-margin-top:110px}.section-kicker{color:var(--accent);font-size:12px;font-weight:700;letter-spacing:.15em;text-transform:uppercase;border-bottom:1px solid var(--line);padding-bottom:12px;margin-bottom:24px}section h1{font-size:30px;line-height:1.2;margin-top:0}h2{margin-top:38px;font-size:24px;line-height:1.3}h3{margin-top:28px;font-size:19px}p{margin:13px 0 18px}a{color:#0b717b}strong{font-weight:700}code{font:0.89em/1.5 ui-monospace,SFMono-Regular,Consolas,monospace;background:var(--wash);border-radius:3px;padding:2px 4px;overflow-wrap:anywhere}pre{background:#132e37;color:#e2f0f1;padding:18px 22px;border-radius:7px;overflow-x:auto;line-height:1.5}pre code{background:none;color:inherit;padding:0;overflow-wrap:normal}table{display:block;max-width:100%;overflow:auto;border-collapse:collapse;margin:22px 0;font-size:14px;line-height:1.5}th,td{padding:12px 14px;text-align:left;border:1px solid var(--line);vertical-align:top}th{background:var(--wash);font-weight:700}tr:nth-child(even) td{background:#fafcfc}td code{white-space:normal}.source{font-size:.9em}li{padding:2px 0}footer{max-width:1100px;margin:0 auto 36px;padding:0 24px;color:var(--muted);font-size:13px}blockquote{margin-left:0;border-left:4px solid var(--accent);padding:6px 22px;background:var(--wash)}
@media(max-width:700px){section{padding:24px 20px}nav{position:static}main{padding:0 10px}header{padding:32px 24px}h2{font-size:21px}table{font-size:12px}th,td{padding:8px}}
@media print{body{background:#fff;font-size:10pt}nav{display:none}header{padding:20px;background:#fff;color:#000;border-bottom:2px solid #333}header p,header .meta,header .eyebrow{color:#333}main{max-width:none;margin:0;padding:0}section{border:0;border-radius:0;padding:16px 0;break-before:page}pre{white-space:pre-wrap;background:#eee;color:#111}table{display:table;font-size:8pt}h1,h2,h3{break-after:avoid}tr{break-inside:avoid}}
'''
page='<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>Spectraphon SP67 — Firmware & DSP Analysis</title><style>'+css+'</style></head><body><header><div class="eyebrow">Leviathan research · firmware reconstruction</div><h1>Spectraphon SP67</h1><p>Architecture, spectral analysis, polynomial synthesis, Array persistence, and a reproducible implementation foundation.</p><div class="meta">28 September 2026 · 178,932 input bytes · 9 extracted tables · 31 passing host tests · hardware equivalence not yet validated</div></header><nav>'+nav+'</nav><main>'+''.join(body)+'</main><footer>Read-only analysis of the user-provided firmware. Analyst labels and mathematical translations are not original source code. See Sources &amp; provenance for scope and attribution.</footer></body></html>'
(ROOT/'Spectraphon_SP67_Analysis.html').write_text(page)
(ROOT/'Spectraphon_SP67_Combined_Analysis.md').write_text('\n\n---\n\n'.join(combined))
print('HTML bytes',len(page.encode()),'combined words',len(' '.join(combined).split()))
