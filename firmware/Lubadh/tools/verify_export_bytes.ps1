# Static ELF data inspection only. No archived ARM instructions or scripts run.
$ErrorActionPreference='Stop'
$researchRoot=Split-Path $PSScriptRoot -Parent
$elfPath=Join-Path $researchRoot 'extracted/bin/audio_save'
$hash=(Get-FileHash -LiteralPath $elfPath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($hash -ne 'd51b574a71cca3a7d66a4b7803ad8f9e9305e69461d2217cc108c17bb506aa23') { throw 'Worker hash mismatch' }
$data=[IO.File]::ReadAllBytes($elfPath)
function Read-U32([int]$offset) { [BitConverter]::ToUInt32($data,$offset) }
function Read-ZString([int]$offset) {
    $end=$offset
    while ($end -lt $data.Length -and $data[$end] -ne 0) { $end++ }
    if ($end -eq $data.Length) { throw 'Missing string terminator' }
    [Text.Encoding]::ASCII.GetString($data,$offset,$end-$offset)
}
if ((Read-U32 0) -ne 0x464c457f -or $data[4] -ne 1 -or $data[5] -ne 1) { throw 'Expected ELF32 little endian' }
$sectionOffset=Read-U32 32
$stride=[BitConverter]::ToUInt16($data,46)
$count=[BitConverter]::ToUInt16($data,48)
$sections=@(for ($i=0; $i -lt $count; $i++) {
    $header=$sectionOffset+$i*$stride
    [pscustomobject]@{name=Read-U32 $header; type=Read-U32 ($header+4);
        address=Read-U32 ($header+12); offset=Read-U32 ($header+16);
        size=Read-U32 ($header+20); link=Read-U32 ($header+24); entry=Read-U32 ($header+36)}
})
$names=$sections[[BitConverter]::ToUInt16($data,50)]
foreach ($section in $sections) { $section.name=Read-ZString ($names.offset+$section.name) }
function Read-VA([uint32]$address) {
    foreach ($section in $sections) {
        if ($section.type -ne 8 -and $address -ge $section.address -and $address+4 -le $section.address+$section.size) {
            return Read-U32 ($section.offset+$address-$section.address)
        }
    }
    throw 'Address outside file-backed sections'
}
# Decode the two ADD immediates and final positive LDR displacement in this PLT stub.
function Decode-Immediate([uint32]$instruction) {
    $value=[uint64]($instruction -band 255)
    $rotate=2*(($instruction -shr 8) -band 15)
    if ($rotate -eq 0) { return [uint32]$value }
    [uint32]((($value -shr $rotate) -bor ($value -shl (32-$rotate))) -band 0xffffffffL)
}
function Resolve-PLT([uint32]$pltAddress) {
$words=@((Read-VA $pltAddress),(Read-VA ($pltAddress+4)),(Read-VA ($pltAddress+8)))
if (($words[0] -band 0xfffff000L) -ne 0xe28fc000L -or
    ($words[1] -band 0xfffff000L) -ne 0xe28cc000L -or
    ($words[2] -band 0xfffff000L) -ne 0xe5bcf000L) { throw 'Unsupported PLT instruction pattern' }
$gotAddress=$pltAddress+8+(Decode-Immediate $words[0])+(Decode-Immediate $words[1])+($words[2] -band 4095)
$rel=$sections | Where-Object name -eq '.rel.plt'
$symbols=$sections[$rel.link]
$strings=$sections[$symbols.link]
$importName=$null
for ($offset=$rel.offset; $offset -lt $rel.offset+$rel.size; $offset+=$rel.entry) {
    if ((Read-U32 $offset) -ne $gotAddress) { continue }
    $info=Read-U32 ($offset+4)
    if (($info -band 255) -ne 22) { throw 'Expected R_ARM_JUMP_SLOT' }
    $symbolOffset=$symbols.offset+($info -shr 8)*$symbols.entry
    $importName=Read-ZString ($strings.offset+(Read-U32 $symbolOffset))
}
if ($null -eq $importName) { throw 'Missing PLT relocation' }
[ordered]@{plt_address=('0x{0:x}' -f $pltAddress); plt_words=@($words | ForEach-Object { '0x{0:x8}' -f $_ });
    got_address=('0x{0:x}' -f $gotAddress); imported_symbol=$importName}
}
$write=Resolve-PLT 0x12108
if ($write.imported_symbol -cne '_ZNSo5writeEPKci') { throw 'Unexpected write import' }
$system=Resolve-PLT 0x1212c
if ($system.imported_symbol -cne 'system') { throw 'Unexpected command import' }
$additional=@(0x121ec,0x123a8,0x1245c,0x1233c,0x123f0 | ForEach-Object { Resolve-PLT $_ })
$callSites=@(foreach ($site in @(0x13240,0x13be8)) {
    $word=Read-VA $site
    if (($word -band 0xff000000L) -ne 0xeb000000L) { throw 'Expected unconditional ARM BL' }
    $displacement=[int64]($word -band 0xffffff)
    if ($displacement -ge 0x800000) { $displacement-=0x1000000 }
    $target=$site+8+4*$displacement
    if ($target -ne 0x1212c) { throw 'Unexpected command call target' }
    # Each normal continuation overwrites r0 with the command string pointer.
    $expected=if ($site -eq 0x13240) { 0xe59d00d0L } else { 0xe59d0138L }
    $next=Read-VA ($site+4)
    if ($next -ne $expected) { throw 'Unexpected normal command continuation' }
    [ordered]@{call_address=('0x{0:x}' -f $site); call_word=('0x{0:x8}' -f $word);
        next_word=('0x{0:x8}' -f $next); target=('0x{0:x}' -f $target);
        normal_continuation_overwrites_return_register=$true}
})
$result=[ordered]@{status='PASS_STATIC_ELF_DATA_ONLY'; worker_sha256=$hash;
    write_import=$write; command_import=$system; additional_imports=$additional;
    command_call_sites=$callSites;
    limitations=@('Only file-backed ELF metadata and PLT instruction encodings inspected.',
        'No ARM worker, file stream, SoX conversion, USB operation or round-trip executed.')}
[IO.File]::WriteAllText((Join-Path $researchRoot 'evidence/export_byte_checks.json'),($result|ConvertTo-Json -Depth 5)+"`n",[Text.UTF8Encoding]::new($false))
[pscustomobject]@{status=$result.status; write=$write.imported_symbol; command=$system.imported_symbol;
    additional=$additional; command_calls=$callSites.Count}|ConvertTo-Json -Depth 5
