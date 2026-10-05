# Gate-to-tape write histories

Real gate events now connect to complete state transitions, input history, head
motion, actual tape writes, channel-fade expiry and both manager updates in one
persistent byte-execution suite. All 48 sequences / 702 callback slices pass;
36,480 tape samples are written. The strengthened suite's 14,118,480 comparisons have zero discrepancy,
with 1,273 distinct instruction addresses covered. These counts describe this
bounded integration, not whole-instrument parity.

## Reproduction and execution boundary

```sh
python firmware/Lubadh/probes/probe_gate_write_history.py
```

Results: `probes/gate_write_history_probe_results.json`, including release and
expiry traces. The shared harness pins the original main ELF to SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
No appliance process or script executes. Diagnostic string/logger services are
inert; state, DSP and pool operations remain original instructions.

Each sequence starts with an initialized playback pool at position 3456 and a
fresh record pool. Complete Channel event 4 enters Overdub through the real
Playback state method and full setState. The original record-head activation
is preserved. Each block separately publishes
original InputBuffer history and moves the original record Tap, then runs the
callback slice from 0x48c58 through channel Fade movement, active-list dispatch,
complete recordInput, expiry, and both TapManager::update calls. Execution stops
at 0x48554 before the callback epilogue. Complete event 5 releases through the
real Overdub state method and setState into Playback.

Fixtures cover speeds -1/+1, durations 128/1000, frames 32/128, feedback 0/0.5/1,
and release after the first block or after fade-in completion plus two blocks.
The looping preset makes channel fade movement use frame count. Tape has 20,000
finite cells; input and initial tape are deterministic nonzero signals. The
original write clipper is configured with knee 1 and compensation 0. All tape
cells, including untouched cells, are compared after every callback.

## Independent model and checked fresh-head state

The reference independently carries channel fade activation, release,
integer/fraction motion, expiry and record-pool lifetime. It predicts tape
indices from the persistent head coordinate and supplied unit speed. It computes
the moving channel fade's interpolated prefix and boundary suffix using the
previously recovered equations, then uses binary32 multiplication/FMA for the
incoming/retained tape blend. At integer resampling phase, the input is the
history buffer's sample i+1. Input phase is checked as zero throughout.

Fresh-head activation leaves all four head fades inactive, with current/previous
coordinates zero and step +1. The strengthened suite predicts and checks all
those fields after gate activation, at every recordInput entry and after every
manager update. Tap movement skips inactive head fades, so their coordinates
remain zero. Head gain is independently one throughout these fresh-pool
histories; no executed endpoint is used to generate expected tape. The reference
tape persists across blocks without replacement by observed output.

This corrects the initial report's assertion that fresh activation created a
per-head fade. Complete activate_new's fresh-engine path calls Tap::activate;
fade triggering appears in other reuse/boundary paths. Those active-fade paths
are checked separately in `TAP_FADE_STATE_FINDINGS.md`, rather than inferred
from this single-head suite.

## Verified ordering and behavior

- Gate assertion enters Overdub and starts the channel write fade at zero.
  Release enters Playback and reverses its step. Release preserves coordinates
  when the fade is active; an inactive fade starts its release at 254.
- Actual recordInput runs before expiry, including the crossing block. The lower
  suffix preserves existing tape; the upper suffix records with ordinary feedback.
- Upper expiry resets the channel fade but leaves the record pool alive. Subsequent
  blocks continue ordinary recording until release completes.
- Lower expiry resets the channel fade and record pool after that block's write.
  Both managers update afterward. The next callback has no record dispatch and
  leaves tape unchanged.
- Fade::move still advances integer/fraction coordinates when its active flag is
  clear. After reset the default step is +1, so inactive coordinates advance by
  frames. The flag suppresses envelope and expiry use; these inactive coordinates
  are not a persistent audible fade. Later activation initializes them anew.

## Remaining specification boundary

This closes the single-head event/state/write/expiry/update integration gap for
the supplied scheduling. It does not recover GPIO-to-callback timing, Link aliases,
active reuse/boundary fade production in this event history, nonunit/held
speed histories, multiple heads, pool exhaustion, region retriggers, input/output
coloration or capacity transitions in this same history. Those remain separate
integration requirements in the readiness ledger. A Rack specification must
also choose its sample-rate/block scheduling policy deliberately; the firmware
fixture's frame-based fade duration is not automatically a Rack-time constant.
