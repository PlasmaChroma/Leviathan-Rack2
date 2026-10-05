#!/usr/bin/env python3
"""Create self-contained HTML views. Requires mistune only for regeneration."""
from pathlib import Path
import re,html
import mistune
ROOT=Path(__file__).resolve().parents[1]
md=mistune.create_markdown(escape=True,plugins=['table'])
css='''
:root{color-scheme:light dark;--bg:#f5f3ef;--paper:#fff;--ink:#24303a;--sub:#5c6871;--line:#d9dfdf;--accent:#285f65;--code:#edf2f2}
@media(prefers-color-scheme:dark){:root{--bg:#14191d;--paper:#1b2227;--ink:#e6e9e9;--sub:#aebcc2;--line:#39444b;--accent:#9cd2c7;--code:#252f35}}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--ink);font:17px/1.65 system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif}
header{border-bottom:1px solid var(--line);padding:22px max(24px,calc((100% - 1080px)/2));background:var(--paper)}header small{letter-spacing:.13em;font-weight:700;color:var(--accent)}nav{font-size:14px;display:flex;gap:20px;flex-wrap:wrap;margin-top:8px}a{color:var(--accent);text-underline-offset:.2em}
main{max-width:1080px;margin:30px auto;padding:40px 50px;background:var(--paper);border:1px solid var(--line);border-radius:10px}h1{font-size:2.15rem;line-height:1.2;margin:0 0 22px;letter-spacing:-.035em}h2{font-size:1.55rem;line-height:1.25;border-top:1px solid var(--line);padding-top:30px;margin-top:44px;letter-spacing:-.02em}h3{font-size:1.15rem;margin-top:29px}p{margin:14px 0}strong{font-weight:700}code{font: .87em/1.5 ui-monospace,SFMono-Regular,Consolas,monospace;background:var(--code);padding:2px 5px;border-radius:4px;overflow-wrap:anywhere}pre{padding:19px 22px;background:var(--code);border-radius:6px;overflow-x:auto;font-size:14px;line-height:1.5}pre code{padding:0;white-space:pre;overflow-wrap:normal}table{border-collapse:collapse;width:100%;font-size:.88rem;margin:22px 0;display:block;overflow-x:auto}th,td{border:1px solid var(--line);padding:10px 12px;vertical-align:top}th{background:var(--code);text-align:left}blockquote{border-left:4px solid var(--accent);margin:20px 0;padding:2px 22px;color:var(--sub)}.contents{font-size:.86rem;border:1px solid var(--line);border-radius:6px;padding:16px 20px;margin:30px 0}.contents summary{font-weight:700;cursor:pointer}.contents ol{columns:2;column-gap:32px;padding-left:20px}.contents li{margin:5px 0;break-inside:avoid}footer{max-width:1080px;margin:20px auto 40px;color:var(--sub);font-size:13px;padding:0 25px}
@media(max-width:700px){main{margin:0;padding:27px 20px;border:0;border-radius:0}h1{font-size:1.85rem}.contents ol{columns:1}body{font-size:16px}}
@media print{body,main,header{background:white;color:black}nav,.contents{display:none}main{border:none;padding:0;max-width:none}pre,table{break-inside:avoid}h2,h3{break-after:avoid}}
'''
for p in (ROOT/'report').glob('*.md'):
 body=md(p.read_text());titles=[]
 def heading(m):
  text=m[1];plain=re.sub('<[^>]*>','',text);slug=re.sub('[^a-z0-9]+','-',html.unescape(plain).lower()).strip('-');titles.append((slug,plain));return f'<h2 id="{slug}">{text}</h2>'
 body=re.sub(r'<h2>(.*?)</h2>',heading,body)
 toc='<details class="contents"><summary>Contents</summary><ol>'+''.join(f'<li><a href="#{s}">{t}</a></li>' for s,t in titles)+'</ol></details>'
 first_h2=body.find('<h2');body=body[:first_h2]+toc+body[first_h2:] if first_h2>=0 else body
 title=html.escape(p.stem.replace('_',' ').title())
 nav='<nav><a href="LUBADH_REVERSE_ENGINEERING.html">Analysis</a><a href="RACK_IMPLEMENTATION_HANDOFF.html">Rack handoff</a><a href="PARAMETER_BIBLE.html">Parameters</a><a href="'+p.with_suffix('.md').name+'">Markdown source</a></nav>'
 doc=f'<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>{title}</title><style>{css}</style></head><body><header><small>LEVIATHAN · FIRMWARE RESEARCH</small>{nav}</header><main>{body}</main><footer>Analyzed artifact: user-supplied Lúbadh 2.1.0 update. Restricted instruction probes and static reconstruction; not full firmware or hardware validation.</footer></body></html>'
 p.with_suffix('.html').write_text(doc)
print('Rendered report HTML files.')
