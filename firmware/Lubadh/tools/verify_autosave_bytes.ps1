# Inspect ELF data only; never execute the appliance worker or its instructions.
param([switch]$SettingsWorker)
$ErrorActionPreference='Stop'
$researchRoot=Split-Path $PSScriptRoot -Parent
$workerName=if ($SettingsWorker) { 'autosave_data' } else { 'autosave_audio' }
$elfPath=Join-Path $researchRoot ('extracted/bin/'+$workerName)
$hash=(Get-FileHash -LiteralPath $elfPath -Algorithm SHA256).Hash.ToLowerInvariant()
$expectedHash=if ($SettingsWorker) { '3b48ea8b2d279a73667dda4871827a613d432e4819fddaecf259ec744eb9af13' } else { '5449c24265304400012bb37d0250ca7dae0cd38c7c5f0e848a3bdc4dcd873622' }
if ($hash -ne $expectedHash) { throw 'Worker hash mismatch' }
$bytes=[IO.File]::ReadAllBytes($elfPath)
function U32([int]$offset) { [BitConverter]::ToUInt32($bytes,$offset) }
function ZString([int]$offset) {
    $end=$offset
    while ($end -lt $bytes.Length -and $bytes[$end]) { $end++ }
    if ($end -eq $bytes.Length) { throw 'Unterminated string' }
    [Text.Encoding]::ASCII.GetString($bytes,$offset,$end-$offset)
}
if ((U32 0) -ne 0x464c457f -or $bytes[4] -ne 1 -or $bytes[5] -ne 1) { throw 'Expected ELF32 little endian' }
$start=U32 32
$stride=[BitConverter]::ToUInt16($bytes,46)
$count=[BitConverter]::ToUInt16($bytes,48)
$sections=@(for ($i=0; $i -lt $count; $i++) {
    $h=$start+$i*$stride
    [pscustomobject]@{name=U32 $h; type=U32 ($h+4); address=U32 ($h+12);
        offset=U32 ($h+16); size=U32 ($h+20); link=U32 ($h+24); entry=U32 ($h+36)}
})
$names=$sections[[BitConverter]::ToUInt16($bytes,50)]
foreach ($s in $sections) { $s.name=ZString ($names.offset+$s.name) }
function FileOffset([uint32]$va) {
    foreach ($s in $sections) {
        if ($s.type -ne 8 -and $va -ge $s.address -and $va+4 -le $s.address+$s.size) {
            return $s.offset+$va-$s.address
        }
    }
    throw 'Address not file backed'
}
function ReadVA([uint32]$va) { U32 (FileOffset $va) }
function Immediate([uint32]$word) {
    $v=[uint64]($word -band 255); $r=2*(($word -shr 8) -band 15)
    if ($r -eq 0) { return [uint32]$v }
    [uint32]((($v -shr $r) -bor ($v -shl (32-$r))) -band 0xffffffffL)
}
$rel=$sections | Where-Object name -eq '.rel.plt'
$symbols=$sections[$rel.link]; $strings=$sections[$symbols.link]
$addresses=if ($SettingsWorker) { @(0x14590,0x140f8,0x143b0) } else { @(0x124e0,0x125c4,0x1263c,0x127e0,0x12894,0x128c4,0x12870) }
$imports=@(foreach ($va in $addresses) {
    $a=ReadVA $va; $b=ReadVA ($va+4); $c=ReadVA ($va+8)
    if (($a -band 0xfffff000L) -ne 0xe28fc000L -or ($b -band 0xfffff000L) -ne 0xe28cc000L -or
        ($c -band 0xfffff000L) -ne 0xe5bcf000L) { throw 'Unknown PLT pattern' }
    $got=$va+8+(Immediate $a)+(Immediate $b)+($c -band 4095)
    $name=$null
    for ($j=$rel.offset; $j -lt $rel.offset+$rel.size; $j+=$rel.entry) {
        if ((U32 $j) -ne $got) { continue }
        $info=U32 ($j+4)
        if (($info -band 255) -ne 22) { throw 'Expected ARM jump slot' }
        $name=ZString ($strings.offset+(U32 ($symbols.offset+($info -shr 8)*$symbols.entry)))
    }
    if ($null -eq $name) { throw 'Missing import' }
    [ordered]@{plt=('0x{0:x}' -f $va); got=('0x{0:x}' -f $got); symbol=$name}
})
if ($SettingsWorker) {
    if ($imports[0].symbol -cne 'nanosleep' -or $imports[1].symbol -cne '__errno_location') { throw 'Unexpected sleep imports' }
} elseif ($imports[0].symbol -cne '_ZNSo5writeEPKci') { throw 'Unexpected write import' }
$literalCases=if ($SettingsWorker) { @(@(0x33f40,'blank'),@(0x33f48,'length'),@(0x33f50,'monitoring'),@(0x33f5c,'playLoop'),@(0x33f68,'recLoop'),@(0x33f70,'modes')) } else { @(@(119048,'autosave/'),@(119060,'audio/'),@(119080,'.dat'),@(119152,'EoD_')) }
$literals=@(foreach ($item in $literalCases) {
    $value=ZString (FileOffset $item[0])
    if ($value -cne $item[1]) { throw 'Literal mismatch' }
    [ordered]@{address=('0x{0:x}' -f $item[0]); value=$value}
})
$result=[ordered]@{status='PASS_STATIC_ELF_DATA_ONLY'; worker_sha256=$hash; imports=$imports; literals=$literals;
    limitations=@('No ARM execution, file operation, original libstdc++ or autosave session tested.',
        'Imported names are provenance for static call analysis, not proof of runtime success.')}
if ($SettingsWorker) {
    $seconds=ReadVA 0x16798; $nanoseconds=ReadVA 0x1679c
    if ($seconds -ne 5 -or $nanoseconds -ne 0) { throw 'Unexpected sleep literal' }
    $result.sleep_literal=[ordered]@{address='0x16798'; seconds=$seconds; nanoseconds=$nanoseconds}
}
$outputName=if ($SettingsWorker) { 'evidence/autosave_settings_byte_checks.json' } else { 'evidence/autosave_byte_checks.json' }
[IO.File]::WriteAllText((Join-Path $researchRoot $outputName),($result|ConvertTo-Json -Depth 6)+"`n",[Text.UTF8Encoding]::new($false))
$imports | ConvertTo-Json -Depth 4
