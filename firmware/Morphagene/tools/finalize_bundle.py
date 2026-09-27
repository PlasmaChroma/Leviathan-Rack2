"""Render this report, refresh artifact status, seal hashes, optionally package.

Does not manufacture validation/compiler results. Run validation/tests separately.
The small HTML renderer supports the Markdown subset used in REPORT.md only.
"""
import argparse,html,json,re,zipfile,subprocess
from pathlib import Path
from common import *

def inline(s):
    pat=r'(`[^`]+`|\*\*[^*]+\*\*|\[[^]]+\]\([^)]+\))'
    out=[]
    for piece in re.split(pat,s):
        if piece.startswith('`'):out.append('<code>'+html.escape(piece[1:-1])+'</code>')
        elif piece.startswith('**'):out.append('<strong>'+html.escape(piece[2:-2])+'</strong>')
        elif piece.startswith('[') and re.fullmatch(r'\[[^]]+\]\([^)]+\)',piece):
            m=re.fullmatch(r'\[([^]]+)\]\(([^)]+)\)',piece)
            out.append('<a href="'+html.escape(m[2],quote=True)+'">'+html.escape(m[1])+'</a>')
        else:out.append(html.escape(piece))
    return ''.join(out)
def render(md):
    lines=md.splitlines();out=[];toc=[];i=0
    while i<len(lines):
        line=lines[i]
        if not line.strip():i+=1;continue
        if line.startswith('```'):
            block=[];i+=1
            while i<len(lines) and not lines[i].startswith('```'):block.append(lines[i]);i+=1
            out.append('<pre><code>'+html.escape('\n'.join(block))+'</code></pre>');i+=1;continue
        m=re.match(r'^(#{1,3}) (.*)',line)
        if m:
            level=len(m[1]);slug=re.sub('[^a-z0-9]+','-',m[2].lower()).strip('-')
            out.append(f'<h{level} id="{slug}">{inline(m[2])}</h{level}>')
            if level==2:toc.append(f'<a href="#{slug}">{html.escape(m[2])}</a>')
            i+=1;continue
        if line.startswith('|'):
            rows=[]
            while i<len(lines) and lines[i].startswith('|'):
                row=[c.strip() for c in lines[i].strip().strip('|').split('|')]
                if not all(re.fullmatch(r':?-+:?',c) for c in row):rows.append(row)
                i+=1
            out.append('<div class="table-wrap"><table><thead><tr>'+''.join('<th>'+inline(c)+'</th>' for c in rows[0])+'</tr></thead><tbody>')
            out.extend('<tr>'+''.join('<td>'+inline(c)+'</td>' for c in row)+'</tr>' for row in rows[1:])
            out.append('</tbody></table></div>');continue
        if re.match(r'^(\d+\. |[-*] )',line):
            kind='ol' if line[0].isdigit() else 'ul';out.append('<'+kind+'>')
            while i<len(lines) and re.match(r'^(\d+\. |[-*] )',lines[i]):
                block=[re.sub(r'^(\d+\. |[-*] )','',lines[i])];i+=1
                while i<len(lines) and lines[i].startswith('   '):block.append(lines[i].strip());i+=1
                out.append('<li>'+inline(' '.join(block))+'</li>')
            out.append('</'+kind+'>');continue
        block=[line];i+=1
        while i<len(lines) and lines[i].strip() and not re.match(r'^(#|\||```|\d+\. |[-*] )',lines[i]):
            block.append(lines[i]);i+=1
        out.append('<p>'+inline(' '.join(block))+'</p>')
    css='''
    :root{color-scheme:light;--ink:#19313c;--accent:#126a65;--line:#dbe5e7}
    *{box-sizing:border-box}body{margin:0;background:#f3f6f6;color:var(--ink);font:16px/1.65 system-ui,sans-serif}
    .layout{max-width:1390px;margin:auto;display:grid;grid-template-columns:250px minmax(0,1fr);gap:32px;padding:40px 30px}
    nav{position:sticky;top:24px;align-self:start;font-size:12px;line-height:1.5}nav strong{display:block;font-size:14px;margin-bottom:16px}
    nav a{display:block;padding:6px 0;text-decoration:none;color:#46636c}nav a:hover{color:var(--accent)}
    main{background:white;border:1px solid var(--line);border-radius:12px;padding:42px 48px;min-width:0}
    h1{font-size:34px;line-height:1.2;letter-spacing:-1px;margin:0 0 28px}h2{font-size:24px;line-height:1.3;margin:44px 0 16px;padding-top:18px;border-top:1px solid var(--line);scroll-margin-top:20px}
    h3{font-size:19px;margin-top:30px}p{margin:14px 0}a{color:var(--accent)}strong{color:#102831}
    code{font:13px/1.6 ui-monospace,Consolas,monospace;background:#edf3f3;padding:2px 4px;border-radius:3px;overflow-wrap:anywhere}
    pre{background:#142c37;color:#ddf0ec;padding:20px;border-radius:8px;overflow:auto}pre code{background:none;padding:0;color:inherit;white-space:pre;overflow-wrap:normal}
    .table-wrap{overflow:auto;margin:22px 0}table{border-collapse:collapse;width:100%;font-size:13px}th{text-align:left;background:#eaf2f1;color:#1b4948}th,td{border:1px solid var(--line);padding:10px 12px;vertical-align:top}td code{font-size:12px}
    li{padding-left:5px;margin:10px 0}footer{margin-top:40px;font-size:12px;color:#658087}
    @media(max-width:900px){.layout{display:block;padding:16px}nav{position:static;margin-bottom:22px}nav a{display:inline-block;margin-right:14px}main{padding:24px}h1{font-size:28px}}
    @media print{body{background:white}.layout{display:block;padding:0}nav{display:none}main{border:0;padding:0}pre{white-space:pre-wrap}h2,h3{break-after:avoid}tr{break-inside:avoid}}
    '''
    return '<!doctype html>\n<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>Morphagene MG204 — audited extension</title><style>'+css+'</style></head><body><div class="layout"><nav><strong>MG204 / Engineering evidence</strong>'+''.join(toc)+'<a href="README.md">Bundle & reproduction</a><a href="CODEX_HANDOFF.md">Continuation handoff</a></nav><main>'+''.join(out)+'<footer>Generated from REPORT.md. Exact bytes, transcribed components, and incomplete behaviors are distinguished in the report.</footer></main></div></body></html>\n'

def files():
    return sorted(p for p in ROOT.rglob('*') if p.is_file() and '__pycache__' not in p.parts and p.name!='MANIFEST.sha256' and p.suffix not in ('.pyc','.exe','.o'))
def run(archive=None):
    (ROOT/'REPORT.html').write_text(render((ROOT/'REPORT.md').read_text(encoding='utf-8')),encoding='utf-8')
    # Original bundle files are fingerprinted against the actual user archive.
    original=ROOT.parent/'Morphagene.zip';comparison=[];baseline_source='original uploaded outer ZIP'
    audit_path=ROOT/'analysis/original_file_audit.json'
    if original.exists():
        with zipfile.ZipFile(original) as z:
            for n in z.namelist():
                if n.endswith('/'):continue
                p=(ROOT.parent/n).resolve()
                if ROOT not in p.parents:raise ValueError('Archive entry lies outside Morphagene')
                comparison.append(dict(path=str(p.relative_to(ROOT)).replace('\\','/'),sha256=sha(z.read(n)),
                                       unchanged=p.exists() and p.read_bytes()==z.read(n)))
    else:
        prior=json.loads(audit_path.read_text()) if audit_path.exists() else {}
        if prior.get('files'):
            baseline_source=prior.get('baseline_source','preserved original-file fingerprints')
            for item in prior['files']:
                p=ROOT/item['path']
                comparison.append(dict(item,unchanged=p.exists() and sha(p.read_bytes())==item['sha256']))
        else:
            # The user can remove the redundant outer ZIP after the initial
            # byte comparison. Original files were already tracked in this repo.
            tracked=subprocess.check_output(['git','-C',str(ROOT),'ls-files','--','.'],text=True).splitlines()
            if not tracked:raise ValueError('No original ZIP, fingerprint list, or tracked baseline')
            subprocess.run(['git','-C',str(ROOT),'diff','--exit-code','--','.'],check=True,capture_output=True)
            baseline_source='Git-tracked original files with no working-tree diff; initial outer-ZIP comparison also passed during analysis'
            comparison=[dict(path=n,sha256=sha((ROOT/n).read_bytes()),unchanged=True) for n in tracked]
    if not all(x['unchanged'] for x in comparison):raise ValueError('An original uploaded file changed')
    put_json(audit_path,dict(baseline_source=baseline_source,
        original_archive_sha256=sha(original.read_bytes()) if original.exists() else None,files=comparison))
    b=flash();ins=json.loads((ROOT/'analysis/reachable_instructions.json').read_text())
    literals={d['literal_address'] for d in ins if 'literal_address' in d}
    entries={int(e['address'],16) for e in json.loads((ROOT/'analysis/function_candidates.json').read_text())}
    old=[]
    for p in sorted((ROOT/'Morphagene_MG204_RE_bundle').glob('decompile_*.c')):
        m=re.search(r'080[0-9a-f]+',p.stem)
        a=(int(m[0],16)&~1) if m else 0x08027518
        containing=next((d for d in ins if d['address']<a<d['address']+d['size']),None)
        status='verified entry' if a in entries else 'interior of decoded instruction' if containing else 'literal pool' if any(l<=a<l+4 for l in literals) else 'not established as function entry'
        old.append(dict(file=p.name,claimed_code_address=hex(a),classification=status,
                        instruction_start=hex(containing['address']) if containing else None,
                        note='Generated C types/expressions are not validated original source'))
    put_json(ROOT/'analysis/legacy_decompilation_audit.json',old)
    put_json(ROOT/'analysis/artifact_status.json',dict(
        input='Original supplied MG204 ZIP and original analysis files retained',
        firmware='Complete transported application; 644 CRC-valid payloads; not full-device flash',
        tables='14 exact blocks / 8056 words; individual roles and active extents qualified in manifest',
        disassembly='Recursive static evidence plus unvalidated broad sweep; unresolved transfers retained',
        reconstruction='Selected finite-input components; not a complete emulator',
        changed_claims=['clock quantizer direction/bin retention','stored Morph fractions','ordered Gene float arithmetic',
                       'conditional recording filter','upper Morph discrete rate selection','adaptive interpolation'],
        checks='Read validation_results.json and component_test_results.json for actual execution',
        not_performed=['hardware measurement','flashing','instruction-accurate CPU emulation','Rack module modification',
                       'plugin build (no plugin code was changed)','Ghidra runtime import validation']))
    put_json(ROOT/'analysis/references.json',[
        dict(kind='local_primary',path='mg204_firmware.zip',sha256=ZIP_HASH),
        dict(kind='primary_instruction_reference',url='https://documentation-service.arm.com/static/6245c734b059dc5ff9a8bdab',
             section='C4.12',use='VFMA/VFMS/VFNMA/VFNMS operation semantics only'),
        dict(kind='quality_reference',path='../Mimeophon/REPORT.md',use='Evidence packaging and reproducibility comparison, not MG204 behavior')])
    # Include the legacy nested manifest too: only the new root manifest excludes itself.
    paths=files()
    paths += [p for p in ROOT.rglob('MANIFEST.sha256') if p.parent!=ROOT]
    paths=sorted(set(paths))
    (ROOT/'MANIFEST.sha256').write_text(''.join(f'{sha(p.read_bytes())}  {p.relative_to(ROOT).as_posix()}\n' for p in paths),encoding='utf-8')
    if archive:
        archive=archive.resolve()
        if archive==original.resolve():raise ValueError('Refusing to overwrite original upload')
        if ROOT==archive.parent or ROOT in archive.parents:raise ValueError('Archive must be outside the bundle tree')
        with zipfile.ZipFile(archive,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for p in paths+[ROOT/'MANIFEST.sha256']:z.write(p,'Morphagene/'+p.relative_to(ROOT).as_posix())
        print(f'Wrote {archive.name}: {len(paths)+1} files; SHA-256 {sha(archive.read_bytes())}')
    print(f'HTML rendered; {len(comparison)} original files unchanged; {len(paths)} file hashes sealed')
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--archive',type=Path);run(p.parse_args().archive)
