# Engine-count gain and eight-call transitions

`probes/probe_engine_gain.py` executes the original callback's gain/mix slice
`0x483b0..0x48450`, including `TapManager::num_engines`. The state initializer
slice `0x3cf0c..0x3cf30` executes once per sequence. **20 sequences, 820 blocks
and 30,832 state/audio comparisons pass with zero numerical discrepancy**.
Another 26 trajectory assertions compare slot occupancy and frame counts.
Coverage totals 703 distinct instruction addresses. Fixtures and the ELF hash
are in `probes/engine_gain_probe_results.json`.

## Target depends on engines, not transition heads

The original count routine counts nonzero entries in the four-entry manager
engine-age list. Additional splice slots within an already counted engine do
not increase this count. The target is calculated by repeated float32
multiplication using literal bits `0x3f51eb85` at `0x4802c`:

```text
base = 0.8100000023841858
target = base ** max(engine_count - 1, 0)
```

The exponent notation describes the law; matching arithmetic uses repeated
multiplication, not a general power function. Approximate targets are:

| Engines | Target |
|---|---|
| 0 or 1 | 1 |
| 2 | 0.81 |
| 3 | 0.6561 |
| 4 | 0.531441 |

This is neither division by head count nor division by engine count. The
gain applies to the shared sum, including multiple transitioning heads within
each engine. The probe checks identical gain trajectories with one versus five
active slots per occupied engine.

## Stateful transition

State resides at Channel+6528:

| Offset from Channel | State |
|---|---|
| 6528 | Current gain, float32 |
| 6532 | Stored increment, float32 |
| 6536 | Remaining increments, integer |
| 6540 | Cached engine count, integer |

The constructor initializer sets all four fields to zero. On an observed count
change, the callback sets `increment = (target - current_gain) / 8`, stores the
new count, and starts eight increments. It immediately applies the first
increment before mixing that block. Each later call with unchanged count
applies one stored increment while the counter is positive. When exhausted,
the gain remains at its accumulated value; there is no final forced assignment
to the ideal target, so float32 accumulation can leave a small residual.

A count change during a transition starts a fresh eight-increment transition
from the current gain. This is a fixed-length linear ramp across calls, rather
than an exponential smoother. Gain is **constant within each block**.

Frame count does not scale the increment. The tested 1/7/32/128-frame blocks
produce identical gain-state trajectories. Explicit zero-frame slice fixtures
also advance it; this does not prove that the real audio host supplies empty
callbacks. Converting the ramp into seconds requires the actual callback block
schedule, which remains to be established.

Because the initialized cached count and gain are both zero, an unchanged
initial count of zero leaves gain at zero. A subsequent nonzero count starts
the ramp. Returning to zero after playback targets unity over eight calls,
even though the raw playback sum may then contain no active heads. The probe
establishes state behavior, not audible hardware behavior for empty playback.

## Additive mixing and validation scope

For each sample, the slice computes the fused operation:

```text
destination = destination + filtered_playback_sum * current_gain
```

The destination is not overwritten. Fixtures supply a nonzero destination to
verify that existing content survives this addition. The source entering this
stage follows the output AntiAlias call statically; this probe begins after
that call and supplies its buffer directly. It stops before downstream output
TapeFilter, compander, allpass, plate, preview and clipping processing.

The two tested count sequences include long settled periods and consecutive
interrupted changes across counts zero through four. Original manager
construction/activation produces each count, but managers are rebuilt per
fixture block: natural head expiry/update scheduling is not established here.

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_engine_gain.py
```

Remaining work is to connect this gain stage to original raw render and output
AntiAlias on persistent objects, join tape writes to rendering, establish
monitoring's destination contribution, and validate the complete output chain
and control/event timing. This recovers an output-level contract without
claiming complete instrument fidelity.

Subsequent [Persistent Playback Pipeline](PERSISTENT_PLAYBACK_PIPELINE.md)
connects rendering, output AntiAlias and this gain/mix stage across persistent
objects. Tape writes, boundaries and downstream coloration remain open.
