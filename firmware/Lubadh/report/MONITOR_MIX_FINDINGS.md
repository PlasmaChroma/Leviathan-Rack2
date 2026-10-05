# Executed input-monitor copy and fade branch

`probe_monitor_mix.py` executes `0x47d80..0x47d98`, including its out-of-line
copy/fade branches, against an independent rounded gain recurrence. **160 cases,
13,568 output comparisons and 640 state/source/bounds assertions pass exactly**.
Execution covers 70 instruction addresses. Results, fixtures and ELF hash are in
`probes/monitor_mix_probe_results.json`.

This recovers the consumer of Channel flags +0x27c/+0x27d and gain +640. Names
below describe the checked branch behavior; flag producers and user gestures
remain open. The callback reaches this branch after input coloration and before
input AntiAlias, so its copied monitor signal and the subsequently filtered
recording input occupy different vectors.

| Monitoring flag | Fade flag | Mix behavior |
|---|---|---|
| 0 | 0 | Retain incoming mix vector |
| 1 | 0 | Replace mix with post-color input |
| 1 | 1 | Copy input, then fade toward gain 1 |
| 0 | 1 | Copy input, then fade toward gain 0 |

This is replacement, not an additive monitor mix. Source input is unchanged by
the branch. Fixtures use equal source/destination frame counts, including empty
blocks; memory past the destination remains untouched.

The step has exact float bits `0x3b8548aa`, value
**0.0040675001218914986 per sample**. Each active sample is multiplied by the
current stored gain, then gain advances by +step with monitoring enabled or
−step otherwise, rounded to float32. If the next gain exceeds 1, clamp it to 1,
clear the fade flag and leave the remaining already copied samples unscaled.
The sample just multiplied is retained. If next gain falls below 0, clamp to 0,
clear the flag and zero the **current** sample plus the remaining suffix. The
asymmetric endpoint rule is checked and belongs in a literal implementation.
An exact gain of 0 or 1 does not trigger these strict inequality exits until
the next advance. Empty blocks preserve gain/fade state.

The fixtures combine 0/7/32/128/257-frame blocks, all flag pairs and eight gain
seeds, including endpoint and deliberately out-of-range branch states. These
are not all demonstrated user-reachable states. They do not recover monitoring
normalization, linked-deck routing or flag/gain activation events.

```sh
python3 firmware/Lubadh/probes/probe_monitor_mix.py
```

Use the existing offline Unicorn/glibc environment. Only existing memory services
supplement the original instructions; no appliance is launched. This is an
isolated routing consumer test, not a complete Channel callback or final
input-monitor/tape-output integration. Keep this scope separate from the
connected input/output/recording suite, whose monitor flags are disabled.
