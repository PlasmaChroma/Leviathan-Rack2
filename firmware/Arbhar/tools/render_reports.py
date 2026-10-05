#!/usr/bin/env python3
"""Render static, self-contained styled report HTML using Pandoc (no web fetch)."""
from pathlib import Path
import re, subprocess, tempfile
R=Path(__file__).resolve().parents[1]
CSS='''<style>
:root{color-scheme:light dark;--bg:#f5f5f2;--paper:#fff;--ink:#152331;--muted:#536674;--line:#d5dde2;--accent:#237b86;--code:#edf3f5;}
*{box-sizing:border-box}html{scroll-behavior:smooth}body{margin:0 auto;padding:3.5rem 2rem 5rem;max-width:1140px;font:17px/1.65 system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;background:var(--bg);color:var(--ink)}
header{border-top:7px solid var(--accent);padding-top:1.3rem;margin-bottom:2rem}header h1{font-size:2.65rem;line-height:1.16;letter-spacing:-.035em}h1{font-size:2.2rem;line-height:1.24;letter-spacing:-.025em}h2{font-size:1.55rem;margin-top:2.7rem;border-bottom:1px solid var(--line);padding-bottom:.5rem}h3{font-size:1.15rem;margin-top:1.8rem}p{margin:.85rem 0}a{color:var(--accent);text-decoration-thickness:1px;text-underline-offset:3px}strong{font-weight:690}code,pre{font-family:ui-monospace,SFMono-Regular,Consolas,monospace;font-size:.86em}code{padding:.14em .3em;background:var(--code);border-radius:4px;overflow-wrap:anywhere}pre{background:var(--code);padding:1.1rem 1.25rem;border:1px solid var(--line);border-radius:8px;overflow-x:auto;line-height:1.5}pre code{padding:0;background:none;overflow-wrap:normal}
table{width:100%;border-collapse:collapse;font-size:.88rem;line-height:1.5;margin:1.4rem 0;display:block;overflow-x:auto}th,td{padding:.7rem .75rem;text-align:left;border-bottom:1px solid var(--line);vertical-align:top;min-width:80px}th{background:var(--code);font-weight:700}tr:nth-child(even) td{background:color-mix(in srgb,var(--code) 45%,transparent)}
#TOC{background:var(--paper);border:1px solid var(--line);border-radius:10px;padding:1.1rem 1.7rem;font-size:.95rem;margin:1.8rem 0 2.8rem}#TOC>ul{columns:2;column-gap:3rem}#TOC li{break-inside:avoid;margin:.3rem 0}#TOC ul ul{font-size:.9em}blockquote{margin:1.2rem 0;border-left:4px solid var(--accent);padding:.3rem 1.2rem;background:var(--paper)}li{margin:.3rem 0}.title{margin-top:.2rem}.bundle-nav{font-size:.88rem;color:var(--muted);border-bottom:1px solid var(--line);padding-bottom:1rem;margin-bottom:2rem}.bundle-nav a{margin-right:1.1rem}.footnote{font-size:.85rem;color:var(--muted)}
@media(max-width:700px){body{padding:1.5rem 1rem;font-size:16px}header h1{font-size:2.05rem}#TOC>ul{columns:1}table{font-size:.78rem}th,td{padding:.5rem}h2{font-size:1.35rem}}
@media(prefers-color-scheme:dark){:root{--bg:#10171f;--paper:#17212c;--ink:#e2eaf0;--muted:#9eafbd;--line:#334451;--accent:#80c9cc;--code:#1d2b36}}
@media print{body{background:white;color:black;max-width:none;padding:0;font-size:10pt}#TOC>ul{columns:2}pre{white-space:pre-wrap}a{color:inherit}h2,h3{break-after:avoid}table{display:table;font-size:8pt}tr{break-inside:avoid}.bundle-nav{display:none}}
</style>'''
nav='<div class="bundle-nav"><strong>ARBHAR / RESEARCH DOSSIER</strong><br><a href="index.html">Bundle index</a><a href="ARBHAR_REVERSE_ENGINEERING.html">Findings</a><a href="RACK_IMPLEMENTATION_HANDOFF.html">Rack handoff</a><a href="PARAMETER_BIBLE.html">Parameters</a><a href="STATE_MACHINES.html">State machines</a><a href="VALIDATION_PLAN.html">Validation</a></div>'
files=['README.md','ARBHAR_REVERSE_ENGINEERING.md','RACK_IMPLEMENTATION_HANDOFF.md','PARAMETER_BIBLE.md','STATE_MACHINES.md','VALIDATION_PLAN.md','NOTICE.md']
with tempfile.TemporaryDirectory() as tmp:
 head=Path(tmp)/'head.html';head.write_text(CSS)
 for name in files:
  source=R/name; target=R/('index.html' if name=='README.md' else source.stem+'.html')
  title=source.read_text().splitlines()[0].lstrip('# ')
  subprocess.run(['pandoc',str(source),'--standalone','--toc','--toc-depth=2','--metadata','title='+title,'--include-in-header',str(head),'-o',str(target)],check=True)
  text=target.read_text()
  # First H1 repeats the metadata title. Keep the navigation, then content title as authored.
  text=re.sub(r'<header id="title-block-header">.*?</header>',nav,text,count=1,flags=re.S)
  for md in files:
   dest='index.html' if md=='README.md' else Path(md).stem+'.html'
   text=text.replace('href="'+md+'"','href="'+dest+'"')
  target.write_text(text)
print('Rendered',len(files),'HTML reports with inline CSS')
