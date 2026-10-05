# Independent numerical domain check; no ARM instructions execute.
$ErrorActionPreference='Stop'
$researchRoot=Split-Path $PSScriptRoot -Parent
$tableEvidence=Get-Content -LiteralPath (Join-Path $researchRoot 'evidence/time_indexed_table_bytes.json') -Raw | ConvertFrom-Json
if ($tableEvidence.main_sha256 -ne '2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4') { throw 'Table evidence hash mismatch' }
# With alpha .5 all sums are integers/halves below 8192, exactly representable
# in binary32. The recurrence is floor((current+invertedInput)/2).
$boundChecks=0
for ($current=0; $current -le 4094; $current++) {
    $minimum=[int][Math]::Floor($current/2.0)
    $maximum=[int][Math]::Floor(($current+4095)/2.0)
    if ($minimum -lt 0 -or $maximum -gt 4094) { throw 'Production ADC invariant failed' }
    $boundChecks+=2
}
$rise=@(); $current=0
for ($tick=0; $tick -lt 16; $tick++) {
    $current=[int][Math]::Floor(($current+4095)/2.0)
    $rise+=$current
}
if ($rise[-1] -ne 4094) { throw 'Full-scale fixed point mismatch' }
$rows=foreach ($table in $tableEvidence.tables) {
    $count=$table.values.Count
    $bins=[int[]]::new($count)
    for ($raw=0; $raw -le 4094; $raw++) {
        $amount=[single]($raw/4095.0)
        $scaled=[single]([double]$amount*$count)
        $index=[int][Math]::Truncate([double]$scaled)
        if ($index -lt 0 -or $index -ge $count) { throw 'Reachable index outside copied table' }
        $bins[$index]++
    }
    $directEndpoint=[int][Math]::Truncate([double][single]([single](4095/4095.0)*$count))
    if ($directEndpoint -ne $count) { throw 'Direct endpoint model mismatch' }
    if (($bins | Measure-Object -Sum).Sum -ne 4095 -or $bins[0] -eq 0 -or $bins[-1] -eq 0) { throw 'Bin coverage mismatch' }
    [ordered]@{name=$table.name; count=$count; raw_domain=@(0,4094); bin_sample_counts=$bins;
               first_value=$table.values[0]; last_value=$table.values[-1]; direct_raw_4095_index=$directEndpoint}
}
$result=[ordered]@{
    status='PASS_NUMERICAL_DOMAIN_ONLY'; source_main_sha256=$tableEvidence.main_sha256
    adc_bound_endpoint_checks=$boundChecks; table_index_checks=4095*$tableEvidence.tables.Count
    rising_full_scale_trace=$rise; tables=@($rows)
    proof='Monotonic alpha=.5 truncated integer recurrence preserves 0..4094 from constructor zero for inverted 12-bit inputs 0..4095. Exact integer/half intermediates make binary32 rounding immaterial to this bound.'
    limitations=@('No original ARM instructions executed; relies on previously executed ADC recurrence/production constructor evidence.',
                  'Valid 12-bit reads and unchanged alpha=.5/current state assumed; alternate writers, malformed values, restoration and direct setter calls require separate audit.',
                  'Safe indices do not prove zero-option event/region behavior, full Time dispatch, or table publication histories.')
}
$outputPath=Join-Path $researchRoot 'evidence/time_control_domain_checks.json'
[IO.File]::WriteAllText($outputPath,($result | ConvertTo-Json -Depth 8)+"`n",[Text.UTF8Encoding]::new($false))
[pscustomobject]@{status=$result.status; adc_bound_endpoint_checks=$boundChecks; table_index_checks=$result.table_index_checks;
                 full_scale_limit=$rise[-1]; tables=$tableEvidence.tables.Count}|ConvertTo-Json
