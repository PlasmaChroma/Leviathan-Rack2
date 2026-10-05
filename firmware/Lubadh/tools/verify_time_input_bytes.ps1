# Archived data and independent arithmetic only; no ARM instructions execute.
$ErrorActionPreference='Stop'
$researchRoot=Split-Path $PSScriptRoot -Parent
$elfPath=Join-Path $researchRoot 'extracted/bin/lubadh_main'
$expectedHash='2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4'
$actualHash=(Get-FileHash -LiteralPath $elfPath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualHash -ne $expectedHash) { throw 'Archived ELF hash mismatch' }
$elfBytes=[IO.File]::ReadAllBytes($elfPath)
if ([BitConverter]::ToUInt32($elfBytes,0) -ne 0x464c457f -or $elfBytes[4] -ne 1 -or $elfBytes[5] -ne 1) { throw 'Expected little-endian ELF32' }
$programOffset=[BitConverter]::ToUInt32($elfBytes,28)
$programStride=[BitConverter]::ToUInt16($elfBytes,42)
$programCount=[BitConverter]::ToUInt16($elfBytes,44)
function Read-ArchivedBytes([uint32]$address,[int]$count) {
    for ($segmentIndex=0; $segmentIndex -lt $programCount; $segmentIndex++) {
        $headerOffset=$programOffset+$programStride*$segmentIndex
        if ([BitConverter]::ToUInt32($elfBytes,$headerOffset) -ne 1) { continue }
        $offset=[BitConverter]::ToUInt32($elfBytes,$headerOffset+4)
        $start=[BitConverter]::ToUInt32($elfBytes,$headerOffset+8)
        $size=[BitConverter]::ToUInt32($elfBytes,$headerOffset+16)
        if ($address -ge $start -and [uint64]$address+$count -le [uint64]$start+$size) {
            return @(for ($i=0; $i -lt $count; $i++) { [int]$elfBytes[$offset+$address-$start+$i] })
        }
    }
    throw 'Address outside file-backed load segments'
}
$maps=foreach ($deck in @(@('A',0,0x3dd60,0x37108),@('B',1,0x3dd50,0x370f8))) {
    $constructor=Read-ArchivedBytes $deck[2] 16
    $reinit=Read-ArchivedBytes $deck[3] 16
    if (($constructor -join ',') -ne ($reinit -join ',')) { throw 'Hardware initializer differs from constructor' }
    $pins=@($constructor[0..3])
    if (@($pins | Where-Object { $_ -gt 7 }).Count) { throw 'Invalid production MCP pin' }
    [ordered]@{deck=$deck[0]; channel_id=$deck[1]; constructor_literal=('0x{0:x}' -f $deck[2]);
        init_hardware_literal=('0x{0:x}' -f $deck[3]); copied_first_16_bytes=$constructor;
        mcp_pins=[ordered]@{speed=$pins[0]; first=$pins[1]; time=$pins[2]; second=$pins[3]}}
}
$cachedZero=Read-ArchivedBytes 0x3d8c0 8
if (@($cachedZero | Where-Object { $_ -ne 0 }).Count) { throw 'Cached Time zero literal mismatch' }
$decodeChecks=0
for ($responseHigh=0; $responseHigh -lt 256; $responseHigh++) {
    for ($responseLow=0; $responseLow -lt 256; $responseLow++) {
        $decoded=(($responseHigh -shl 8) -band 0xf00) -bor $responseLow
        if ($decoded -lt 0 -or $decoded -gt 4095) { throw 'MCP decoder domain failed' }
        $decodeChecks++
    }
}
$result=[ordered]@{status='PASS_DATA_AND_NUMERICAL_ONLY'; main_sha256=$actualHash;
    deck_maps=@($maps); cached_time_constructor_literal=$cachedZero; mcp_response_pairs_checked=$decodeChecks;
    decoded_domain=@(0,4095); invalid_pin_return=65535; inverted_invalid_pin_value=-61440;
    limitations=@('Instruction interpretation and destination offsets are established by static disassembly, not executed here.',
        'Constructor/initHardware install valid pins; arbitrary alternate writers and restored/corrupted state are not excluded.',
        'Does not execute SPI, ADC, Time dispatch, or firmware hardware initialization.')}
$outputPath=Join-Path $researchRoot 'evidence/time_input_byte_checks.json'
[IO.File]::WriteAllText($outputPath,($result | ConvertTo-Json -Depth 8)+"`n",[Text.UTF8Encoding]::new($false))
[pscustomobject]@{status=$result.status; decoded_response_pairs=$decodeChecks; deck_a_time_pin=$maps[0].mcp_pins.time; deck_b_time_pin=$maps[1].mcp_pins.time}|ConvertTo-Json
