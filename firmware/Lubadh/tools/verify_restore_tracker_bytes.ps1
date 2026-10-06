# File-backed instruction/data bytes plus an independent predicate enumeration.
# Does not run ARM, streams, exception handlers or startup restoration.
$ErrorActionPreference='Stop'
$researchRoot=Split-Path $PSScriptRoot -Parent
$path=Join-Path $researchRoot 'extracted/bin/lubadh_main'
$hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
if ($hash -ne '2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4') { throw 'Main hash mismatch' }
$data=[IO.File]::ReadAllBytes($path)
function U32([int]$offset) { [BitConverter]::ToUInt32($data,$offset) }
if ((U32 0) -ne 0x464c457f -or $data[4] -ne 1 -or $data[5] -ne 1) { throw 'Expected ELF32 little endian' }
$phoff=U32 28; $stride=[BitConverter]::ToUInt16($data,42); $count=[BitConverter]::ToUInt16($data,44)
function VAWord([uint32]$va) {
    for ($i=0; $i -lt $count; $i++) {
        $h=$phoff+$i*$stride
        if ((U32 $h) -ne 1) { continue }
        $base=U32 ($h+8); $size=U32 ($h+16)
        if ($va -ge $base -and $va+4 -le $base+$size) { return U32 ((U32 ($h+4))+$va-$base) }
    }
    throw 'Address outside file-backed load segments'
}
$expected=@(@(0x2b358,0xe5dd3023L),@(0x2b35c,0xe2433031L),@(0x2b360,0xe3530001L),
    @(0x2b364,0x8a0008acL),@(0x2b270,0xe30a38c0L),@(0x2b274,0xe3403708L),
    @(0x2b4cc,0xe59d2014L),@(0x2b4d4,0xebffab3dL),@(0x2b4d8,0xe1a00005L))
$words=@(foreach ($pair in $expected) {
    $actual=VAWord $pair[0]
    if ($actual -ne $pair[1]) { throw 'Instruction byte mismatch' }
    [ordered]@{address=('0x{0:x}' -f $pair[0]); word=('0x{0:x8}' -f $actual)}
})
$accepted=@(); $rejected=@()
for ($byte=0; $byte -lt 256; $byte++) {
    $difference=([int64]$byte-49+0x100000000L) -band 0xffffffffL
    $branchReject=$difference -gt 1
    $independentAccepted=$byte -eq 49 -or $byte -eq 50
    if ((!$branchReject) -ne $independentAccepted) { throw 'Predicate mismatch' }
    if ($branchReject) { $rejected+=$byte } else { $accepted+=$byte }
}
$target=0x2b364+8+4*0x8ac
if ($target -ne 0x2d61c) { throw 'Branch target mismatch' }
$keys=@(foreach ($pair in @(@(0x71278,'blank'),@(0x71280,'length'),@(0x71288,'modes'),
    @(0x71290,'monitoring'),@(0x7129c,'playLoop'),@(0x712a8,'recLoop'),@(0x712b0,'time'),
    @(0x712b8,'active'),@(0x712c0,'clkDiv'),@(0x712c8,'feedback'),@(0x712d4,'xfade'),
    @(0x712dc,'slew'),@(0x712e4,'tape'),@(0x712ec,'file'),@(0x712f4,'folder'),
    @(0x712fc,'quantiseFlag'),@(0x7130c,'quantiseGrid'))) {
    $text=''; $at=[uint32]$pair[0]
    while ($true) {
        $value=(VAWord $at) -band 255
        if ($value -eq 0) { break }
        $text+=[char]$value; $at++
        if ($text.Length -gt 64) { throw 'Unexpected key length' }
    }
    if ($text -cne $pair[1]) { throw 'Settings key mismatch' }
    [ordered]@{address=('0x{0:x}' -f $pair[0]); key=$text}
})
$result=[ordered]@{status='PASS_BYTES_AND_INDEPENDENT_PREDICATE_ONLY'; main_sha256=$hash;
    checked_words=$words; rejected_branch_target=('0x{0:x}' -f $target);
    accepted_tracker_bytes=$accepted; rejected_count=$rejected.Count; enumerated_count=256;
    requested_raw_read_bytes=0x0708a8c0; settings_key_literals=$keys;
    limitations=@('No original ARM instructions, stream reads or exception paths executed.',
        'Enumeration covers byte-value acceptance only; it says nothing about failed get() destination contents or file validity.')}
[IO.File]::WriteAllText((Join-Path $researchRoot 'evidence/restore_tracker_byte_checks.json'),($result|ConvertTo-Json -Depth 6)+"`n",[Text.UTF8Encoding]::new($false))
[pscustomobject]@{status=$result.status; accepted=$accepted; rejected=$rejected.Count; requested_bytes=$result.requested_raw_read_bytes}|ConvertTo-Json
