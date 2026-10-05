# Archived bytes and independent string-policy checks only; no ARM executes.
$ErrorActionPreference='Stop'
$researchRoot=Split-Path $PSScriptRoot -Parent
$elfPath=Join-Path $researchRoot 'extracted/bin/audio_load'
$hash=(Get-FileHash -LiteralPath $elfPath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($hash -ne '2dd4523f1c846c14952b301aaacb509ed13125fbbd2e0e49c8a1634a1139301a') { throw 'Worker hash mismatch' }
$elfBytes=[IO.File]::ReadAllBytes($elfPath)
if ([BitConverter]::ToUInt32($elfBytes,0) -ne 0x464c457f -or $elfBytes[4] -ne 1 -or $elfBytes[5] -ne 1) { throw 'Expected little-endian ELF32' }
$programOffset=[BitConverter]::ToUInt32($elfBytes,28)
$programStride=[BitConverter]::ToUInt16($elfBytes,42)
$programCount=[BitConverter]::ToUInt16($elfBytes,44)
$tagBytes=$null
$savedFolderBytes=$null
for ($i=0; $i -lt $programCount; $i++) {
    $header=$programOffset+$i*$programStride
    if ([BitConverter]::ToUInt32($elfBytes,$header) -ne 1) { continue }
    $start=[BitConverter]::ToUInt32($elfBytes,$header+8)
    $size=[BitConverter]::ToUInt32($elfBytes,$header+16)
    if (0x22f78 -ge $start -and 0x22f84 -le $start+$size) {
        $offset=[BitConverter]::ToUInt32($elfBytes,$header+4)+0x22f78-$start
        $tagBytes=@($elfBytes[$offset..($offset+11)])
    }
    if (0x22f98 -ge $start -and 0x22f9f -le $start+$size) {
        $offset=[BitConverter]::ToUInt32($elfBytes,$header+4)+0x22f98-$start
        $savedFolderBytes=@($elfBytes[$offset..($offset+6)])
    }
}
if ($null -eq $tagBytes) { throw 'Tags outside file-backed segments' }
if ($null -eq $savedFolderBytes -or $savedFolderBytes[6] -ne 0 -or
    [Text.Encoding]::ASCII.GetString([byte[]]$savedFolderBytes,0,6) -cne 'saved/') { throw 'Saved-folder literal mismatch' }
$tags=@(for ($i=0; $i -lt 3; $i++) {
    if ($tagBytes[$i*4+3] -ne 0) { throw 'Missing tag terminator' }
    [Text.Encoding]::ASCII.GetString([byte[]]$tagBytes,4*$i,3)
})
if (($tags -join ',') -cne '{n},{s},{c}') { throw 'Unexpected tag literals' }
$cases=@(@('plain.wav',0,0),@('plain.wav',1,1),@('x{c}.wav',0,2),
    @('x{s}.wav',0,1),@('x{n}.wav',1,0),@('{c}{s}{n}.wav',1,0),
    @('{c}{s}.wav',0,1),@('x{C}.wav',1,1),@('x{c}suffix',1,2))
$checked=foreach ($case in $cases) {
    $policy=[int]($case[1] -ne 0)
    for ($i=0; $i -lt $tags.Count; $i++) {
        if ($case[0].IndexOf($tags[$i],[StringComparison]::Ordinal) -ge 0) { $policy=$i; break }
    }
    if ($policy -ne $case[2]) { throw 'Independent string-policy case failed' }
    [ordered]@{filename=$case[0]; folder_index=$case[1]; policy=$policy}
}
$result=[ordered]@{status='PASS_DATA_AND_INDEPENDENT_POLICY_ONLY'; worker_sha256=$hash;
    tag_literal_address='0x22f78'; literal_bytes=$tagBytes; ordered_tags=$tags; cases=@($checked);
    saved_folder_literal_address='0x22f98'; saved_folder_literal_bytes=$savedFolderBytes;
    limitations=@('No original ARM string constructor/search or main-loop instructions executed.',
        'Policy model and saved-folder insertion follow static disassembly; real directory/filename histories remain unverified.')}
[IO.File]::WriteAllText((Join-Path $researchRoot 'evidence/import_tag_byte_checks.json'),($result|ConvertTo-Json -Depth 8)+"`n",[Text.UTF8Encoding]::new($false))
[pscustomobject]@{status=$result.status; tags=$tags; independent_cases=$checked.Count}|ConvertTo-Json
