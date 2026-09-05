# Octavia: practical in-Rack features

Status: proposed product and implementation spec; no features in this document
are implemented merely by being listed here.

Derived from [Octavia-Astra.md](Octavia-Astra.md). The original document remains
the architectural rationale. This spec narrows it to things a musician can use
inside Rack, without an external agent.

## Product decision

Make Octavia a small patch inspection and experiment station. The user should
be able to name a connected probe, inspect one polyphonic voice, capture a
gesture, change the patch manually, and compare another gesture.

Keep the current panel width, Master L/R, probes A–D, and Control A/B. Open a
floating **Monitor workspace** by clicking the octopus, with an equivalent
context-menu command for discoverability. This click must not toggle the server;
Start / Stop remains the server control. The workspace is a UI proposal requiring
an in-Rack layout review before final styling.

The workspace operates locally even when HTTP is stopped or another Octavia
owns the port. The octopus brightness continues to mean HTTP server ownership;
it does not indicate whether local observation works. Show server status
separately from capture status in the workspace.

## 1. Named probes and polyphonic voice selection

The first page has six rows: Master L, Master R, A, B, C, D. Each row shows:

| Field | Behavior |
| --- | --- |
| Name | Editable alias, at most 32 Unicode code points; physical name always visible |
| Connection | Unpatched, available channel count, or selected channel unavailable |
| Voice | One selected physical channel, shown as 1–16 to the musician |
| Interpretation | Audio, Gate, Pitch CV, or Envelope; Master rows remain Audio |
| Reading | Small interpretation-specific result and activity indicator |

Examples: `A · Bass gate · voice 3 · Gate`, `B · Bass envelope · voice 3 · Envelope`.

Aliases describe inputs, not ownership of upstream modules. Do not infer a
complete instrument or rewrite cable routing from a name. Duplicated aliases are
allowed because physical probe identity remains explicit.

One selected voice per physical input is the deliberate first limit. It preserves
six sampled streams. Simultaneous voices 1, 4, and 8 of one source require patching
that source to three probes, selecting a different voice on each. No hidden reads
of other modules and no automatic cable changes.

Internal/API channel indices are zero-based. Old patches and requests default
to channel 0. An unavailable selected channel is reported as unavailable, never
silently replaced by channel 0. Retain physical connectivity and selected-channel
availability as separate facts in capture metadata.

Changing voice or interpretation starts a new observation configuration generation.
Snapshots cannot combine samples from different generations. A snapshot that
requires older unavailable history returns `insufficient_history`. Disable these
edits during an armed or active capture; external requests receive a conflict
instead of changing its meaning halfway through. Aliases may change, but a take
keeps the alias frozen when it was armed.

Master meters follow the selected Master voices; label the selections in the
workspace so the readings cannot be mistaken for a sum of all voices.

## 2. A signal inspector, with different meanings for different voltages

Select a row to open its graph and details. Simple live readings update at no
more than 15 Hz. Detailed analysis runs only on request against a frozen window.

| Interpretation | Initial useful display | Explicit limits |
| --- | --- | --- |
| Audio | Peak, RMS, clipping fraction, DC, waveform; spectrum on request | Reuse existing analysis; do not estimate musical quality |
| Gate | State, rising-edge count, last/median pulse width, interval, duty cycle | Default rise ≥1 V and fall ≤0.1 V; hysteresis; high-at-window-start is not a new rising edge |
| Pitch CV | Voltage, nearest note, cents deviation from selected tuning | Default 1 V/oct, 0 V = C4, A4 = 440 Hz; show that convention; label continuous glides as moving |
| Envelope | Voltage trace, min/max, rise/fall slopes, largest discontinuity | Do not infer authored ADSR stages or release time without a trigger/reference definition |

Pitch CV analysis groups consecutive samples into stable regions using an explicit
cents tolerance and minimum duration. Initial defaults: ±5 cents for at least
20 ms, with slope also below 1 semitone/second. Implement off the audio thread.
Values are measured CV, not an estimate of oscillator pitch; downstream tuning
may differ. Include the convention and thresholds in results.

Gate intervals cut by either window boundary are marked censored and excluded
from complete-pulse statistics. No edges means `no_edges`, not zero frequency.

An optional **Clock** view of a Gate probe accepts a user-declared pulses-per-quarter
value and reports median BPM plus interval spread. It must not guess PPQN or
auto-configure Sibyl/Moirai. The view is observational and does not supply transport.

Detailed envelope timing relative to another gate probe is a later addition. The
first version must not label the last downward slope as an authored release.

## 3. Capture a gesture locally

The workspace supplies a compact capture strip:

`Sources · Duration · Start condition · Capture / Cancel · Status`

- Sources: any subset of the six selected streams; default Master L/R.
- Duration: 0.1–30 seconds, bounded by the existing recording limits.
- Start condition: immediately, or next rising edge of a selected Gate probe.
- Armed timeout: 10 seconds initially; timeout leaves the previous take intact.
- Cancellation: disarm immediately at the next audio-frame command boundary;
  discard the incomplete candidate and release buffers off the audio thread.

For immediate capture, the first sample is the frame where the audio thread
adopts the request, not the UI click timestamp. For edge capture, the rising-edge
frame is the first recorded sample. Require a low state before accepting an edge
if the gate was already high when armed. Freeze the trigger voice, thresholds,
source selection, and interpretation settings with the request.

Keep the existing one-active-capture limit. HTTP and local UI requests use one
shared admission path, with a clear busy result. UI code must call the shared
capture service directly; it must not send HTTP to localhost and accidentally
control a different Octavia instance.

Disconnected selected sources at arm time are rejected. A source becoming
unavailable during capture is represented by zeros plus an availability record,
and the result is marked incomplete. A sample-rate change fails the capture.
The user can inspect an incomplete result but cannot mistake it for a clean
comparison.

Stopping the HTTP server does not cancel a local capture. A capture already
accepted over HTTP also completes locally; stopping the server only removes
remote access. Removing the module cancels its work and joins its workers.

## 4. Reference and candidate takes

Provide two slots, **Reference** and **Candidate**, separate from physical probes
A and B. Capture initially fills Candidate. **Use as reference** moves a completed
candidate into Reference; **Compare** analyzes the selected streams in both.

A new candidate replaces the old one only after successful completion. Reference
is never automatically replaced. Retain at most two completed raw takes and one
in-progress candidate. This requires explicit memory accounting in addition to the
existing active-capture limit: six channels × 192 kHz × 30 s × four bytes is about
132 MiB per take. Initial retained-raw budget: 192 MiB per module, covering these
slots and the pending candidate together. Reject a capture before arming if it
would exceed the budget; suggest fewer channels or a shorter duration. Do not
silently downsample evidence or evict Reference. Existing snapshot/history storage
is separate and must also appear in the implementation's memory accounting.

Each take records:

- Local take ID and observation configuration generation.
- Source names, selected physical channels, interpretation and calibration.
- Sample rate, exact half-open frame range `[start, end)`, trigger frame if any.
- Connection/availability changes and completion status.
- A short user note and optional exported artifact references.

Compare shows absolute level changes alongside level-normalized spectral changes
for Audio. Gate compares edge/pulse statistics; Pitch CV compares stable regions;
Envelope compares voltage and shape statistics. Do not apply audio loudness
analysis to CV. All comparisons retain the raw unnormalized evidence.

The first version compares complete windows and reports differing lengths,
trigger policies, source configurations, or unavailable samples. It does not
time-warp recordings, claim identical performances, or infer which patch edit
caused a difference. A warning does not prevent deliberate comparison; the user
can inspect the mismatch. Different signal interpretations prohibit automatic
metric comparison until the user selects a common interpretation.

**Export** writes the existing raw-voltage WAV and sidecar on a worker. Offer an
additional Audio-only listening WAV with fixed `audio_sample = volts / 5`,
explicit calibration, and overload metadata. No independent per-take peak
normalization. CV exports remain voltage data. Do not add playback outputs in
this milestone; users audition their live patch or open exported audio.

## Persistence and panel integration

Add versioned module JSON for aliases, voice selections, interpretations, thresholds,
pitch convention, and capture preferences. Do not serialize raw buffers, active
operations, listener ownership, or ephemeral take IDs. Reload starts idle with
empty take slots and the normal explicit server ownership attempt. Unknown fields
are ignored; unsupported enums fall back to documented defaults. Invalid channel
selections fall back on load, but live unavailable channels remain explicit.

Preserve all parameter, port, and light IDs. No new ports or panel controls are
needed for this proposal. Expose workspace actions through the context menu as
well as the octopus click. Keep the current attention meaning of A–D LEDs; use
workspace text for capture status rather than silently reinterpreting those LEDs.

The master SVG remains the source for artwork, labels, and anchors. Preserve the
split panel/labels pipeline and existing mipmapped/framebuffer-cached octopus.
Use shared graphics lifecycle helpers and cached static chrome. Plot geometry is
decimated on a worker; draw only bounded published points. Cap each plot at 1024
points and at most six visible traces; render no spectra unless requested.

## Engineering ownership and real-time limits

- `Octavia.cpp`: port reads, command adoption, compact telemetry, existing integration.
- `OctaviaObservation.*`: selected-channel availability and configuration-generation
  provenance; immutable snapshot boundaries.
- `OctaviaAnalysis.*`: typed off-thread analyzers and comparison results.
- `OctaviaRecording.*`: shared capture admission, cancellation, trigger policy,
  take retention, and memory reservation.
- Proposed `OctaviaMonitorWorkspace.*`: Rack UI, persistent preferences, and plotting.

Do not put the workspace implementation into an even larger `Octavia.cpp`.
The exact new file split can change, but module, service, and UI ownership must
remain explicit. HTTP handlers and the local workspace share validation/services.

Per audio frame, keep six selected input samples and at most six simple gate
detectors. No allocation, locks, JSON, filesystem work, FFT, note formatting, or
HTTP calls. Publish fixed-size configuration and requests through the existing
mailbox/queue patterns with acknowledgement generations. Cap pending local
capture requests at one and reject duplicate admission while busy.

Reference/Candidate ownership must be explicit; do not accidentally multiply the
memory budget through copied vectors in status messages or UI snapshots. Export
and analysis jobs hold bounded shared immutable references and count toward
retention until finished.

## Delivery order and acceptance gates

1. **Probe workspace:** local popup, aliases, channel selection, connectivity,
   Audio readings, JSON persistence. Gate: two voices carrying different signals
   show the selected voice; disconnect/channel loss is explicit; reconfiguration
   cannot contaminate a snapshot; it works with HTTP stopped.
2. **Typed inspector:** Gate, Pitch CV, Envelope, optional Clock view. Gate:
   synthetic tests cover boundary-cut pulses, constant CV, pitch glides, gate
   hysteresis, known pulse density, and interpretation-specific results.
3. **Local capture:** immediate/edge start, timeout/cancel, shared UI/HTTP admission.
   Gate: deterministic stimulus captures exact frame boundaries; held-high gates
   do not falsely trigger; sample-rate changes, busy requests, and cancellation
   produce the specified results.
4. **Two-take comparison:** slots, memory budget, typed deltas, export. Gate:
   a pure gain change changes absolute level but leaves normalized spectral shape
   approximately unchanged; a shortened envelope changes measured shape; failed
   captures preserve both completed slots; export calibration is verified.

At each stage run focused native tests and a Windows plugin build. UI acceptance
also requires a real Rack check at different zoom/DPI settings and after graphics
context recreation. Audio overhead must be benchmarked against the pre-feature
module at 48/96/192 kHz; publish Process/Step/Draw timings without redefining their
module-level meaning. Any new continuous cost must be justified before release.

First user acceptance exercise: patch a gate to A and its envelope to B; name
them; select the same polyphonic voice; inspect the gate; capture an envelope;
shorten its release manually; capture again; compare and export. No agent,
sequencer modification, or automatic patch restoration is required.

## Deferred from the Astra roadmap

Named multi-module instruments, natural-language constraints, score transformations,
automatic Sibyl/Moirai edits, coordinated revision adoption, prepared transactions,
and conflict-aware rollback remain separate projects. They can consume these local
observations later. The two-take UI does not offer an **Accept/Revert patch** button
because recordings alone cannot establish what state is safe to restore.

Control A/B retain their bounded diagnostic role. Continuous musical sequencing
and automation remain in Sibyl or the user's sequencer. Exact module identities,
request schemas, and display-aware parameter writes remain worthwhile bridge
work, but are not dependencies of the first local probe workspace.
