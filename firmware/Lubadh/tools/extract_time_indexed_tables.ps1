# Read archived ELF data only. No firmware instructions or WSL processes execute.
$ErrorActionPreference = 'Stop'
$researchRoot = Split-Path $PSScriptRoot -Parent
$elfPath = Join-Path $researchRoot 'extracted/bin/lubadh_main'
$expectedHash = '2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4'
$actualHash = (Get-FileHash -LiteralPath $elfPath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualHash -ne $expectedHash) { throw 'Archived ELF hash mismatch' }
$elfBytes = [IO.File]::ReadAllBytes($elfPath)
if ([BitConverter]::ToUInt32($elfBytes,0) -ne 0x464c457f -or $elfBytes[4] -ne 1 -or $elfBytes[5] -ne 1) {
    throw 'Expected little-endian ELF32'
}
$programOffset = [BitConverter]::ToUInt32($elfBytes,28)
$programStride = [BitConverter]::ToUInt16($elfBytes,42)
$programCount = [BitConverter]::ToUInt16($elfBytes,44)
$loadSegments = for ($segmentIndex=0; $segmentIndex -lt $programCount; $segmentIndex++) {
    $headerOffset = $programOffset+$programStride*$segmentIndex
    if ([BitConverter]::ToUInt32($elfBytes,$headerOffset) -eq 1) {
        [pscustomobject]@{
            offset=[BitConverter]::ToUInt32($elfBytes,$headerOffset+4)
            address=[BitConverter]::ToUInt32($elfBytes,$headerOffset+8)
            fileSize=[BitConverter]::ToUInt32($elfBytes,$headerOffset+16)
        }
    }
}
function Read-ArchivedWords([uint32]$address,[int]$count) {
    foreach ($segment in $loadSegments) {
        if ($address -ge $segment.address -and [uint64]$address+4*$count -le [uint64]$segment.address+$segment.fileSize) {
            $fileOffset=$segment.offset+$address-$segment.address
            return @(for ($wordIndex=0; $wordIndex -lt $count; $wordIndex++) {
                [BitConverter]::ToInt32($elfBytes,$fileOffset+4*$wordIndex)
            })
        }
    }
    throw ('Address outside file-backed load segments: {0:x}' -f $address)
}
$tableDefinitions = @(
    @('clock_all',0x72f4c,17), @('clock_even',0x72f90,11),
    @('clock_odd',0x72fbc,7), @('clock_powers_of_two',0x72fd8,8),
    @('constructor_quantisation',0x72ff8,7),
    @('quantisation_all',0x7305c,16), @('quantisation_even',0x72f90,11),
    @('quantisation_odd',0x7309c,6), @('quantisation_powers_of_two',0x72ff8,7)
)
$tables = foreach ($definition in $tableDefinitions) {
    $words = Read-ArchivedWords $definition[1] $definition[2]
    if ($words.Count -ne $definition[2] -or $words[0] -ne 0) { throw 'Table shape mismatch' }
    [ordered]@{name=$definition[0]; address=('0x{0:x}' -f $definition[1]); copied_word_count=$definition[2]; values=$words}
}
$evidence = [ordered]@{
    status='EXTRACTED_DATA_ONLY'; main_sha256=$actualHash; tables=@($tables)
    scope='ELF data bytes read through file-backed PT_LOAD mapping; copied lengths established separately from static constructors/setter instructions.'
    limitations=@('No original instructions executed; vector allocation and control endpoint behavior not dynamically checked.',
                  'Clock arrays include a leading zero absent from the earlier documented musical lists; it is not a trailing bounds sentinel.',
                  'Preset quantisation arrays and constructor default are extracted; complete preset transition and zero-value consumers remain unexecuted.')
}
$outputPath=Join-Path $researchRoot 'evidence/time_indexed_table_bytes.json'
$json=($evidence | ConvertTo-Json -Depth 8)+"`n"
[IO.File]::WriteAllText($outputPath,$json,[Text.UTF8Encoding]::new($false))
Write-Output $json
