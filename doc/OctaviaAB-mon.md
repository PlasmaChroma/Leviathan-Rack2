# Octavia A/B Monitoring Notes

## Intent

Add a second stereo monitoring pair to Octavia so an AI can compare two points
in a patch rather than observing only one stereo signal.

The additional inputs should be treated as named monitoring buses:

- **Monitor A L/R** — normally the source, upstream signal, stem, or reference.
- **Monitor B L/R** — normally the processed signal, downstream result, mix, or
  comparison target.

This is more useful than presenting four unrelated analyzer ports because it
gives the agent a clear before/after measurement contract.

## Useful Workflows

- Compare a signal before and after a filter, compressor, effect, or mastering
  chain.
- Compare an individual stem with the full mix.
- Locate the stage where clipping, noise, excess bass, DC, or stereo problems
  enter a signal chain.
- Measure whether an Octavia edit improved the intended property without
  causing an undesirable loudness or spectral change.
- Observe a sidechain or control source alongside the audio behavior it causes.

An ideal agent result could say, for example, that B reduced a stable 240 Hz
resonance by 5 dB relative to A while preserving programme loudness.

## Performance Model

Octavia currently separates inexpensive continuous capture from expensive
on-demand analysis:

- The audio thread writes input samples into atomic ring buffers and performs
  continuous stereo loudness/K-weighting work.
- Detailed spectrum and problem-frequency analysis runs on the server thread
  only when requested.

Preserve this division. Adding two more ring buffers should be inexpensive;
adding detailed four-channel analysis to the per-sample path would not be.

Suggested safeguards:

- Always permit cheap capture for connected A and B inputs.
- Keep FFT/spectral/problem analysis request-driven.
- Skip all optional work for disconnected inputs.
- Do not run multiple expensive analyses concurrently; serialize or coalesce
  requests.
- Consider continuous loudness for A only initially, or enable the B loudness
  meter only while B is connected and comparison monitoring is active.
- Measure audio-thread and request-time costs at common Rack sample rates before
  enabling continuous metering for both pairs.
- Return measurement window and sample age so an agent can judge whether the A
  and B observations are temporally comparable.

The existing detailed analyzer uses repeated Goertzel scans over 4096-sample
snapshots. That is reasonable for occasional single-port inspection, but a
shared FFT would scale better for matched analysis of A-L, A-R, B-L, and B-R.
Consider the FFT conversion before or alongside four-channel detailed analysis.

## Proposed Agent Contract

Retain the ability to inspect an individual bus or channel, but add a semantic
A/B comparison operation. A future MCP tool could resemble:

```text
vcv_compare_audio(a="A", b="B", include_spectrum=false)
```

The response should use matched snapshots and report both absolute measurements
and B-minus-A deltas where meaningful:

- RMS, peak, crest factor, clipping, and DC offset;
- momentary, short-term, and integrated loudness;
- broad spectral-band levels;
- dominant or stable resonances;
- noise floor, hum, and suspected artifacts;
- stereo balance, correlation, and side-to-mid ratio;
- connection state, measurement duration, and snapshot age.

Detailed raw spectra should remain opt-in to control CPU and response size.

## UI and Compatibility Notes

- Relabel the existing inputs as **MONITOR A L/R** and add **MONITOR B L/R**.
- Make the A/B identity visible on the panel rather than relying only on port
  ordering.
- Append new input IDs rather than inserting them if Octavia's patch
  compatibility needs to be preserved.
- Decide whether loudness reset applies to both buses or accepts a bus selector;
  a selector is clearer for independent measurements, while an `all` option is
  useful before comparisons.
- Display or API status should make it clear when a bus is disconnected or has
  insufficient measurement history.

## Possible Implementation Sequence

1. Add Monitor B L/R inputs and cheap ring-buffer capture.
2. Expose individual B channel inspection without adding continuous DSP.
3. Add synchronized A/B snapshots and a compact comparison response.
4. Benchmark request-time analysis and audio-thread overhead.
5. Replace or supplement repeated Goertzel scans with a shared FFT if needed.
6. Add optional dual-bus loudness measurement after profiling.
7. Add MCP documentation and tests for disconnected, silent, clipped, mono,
   stereo, and temporally mismatched inputs.

## Open Questions

- Should both buses always maintain loudness history, or should B metering be
  explicitly armed?
- Should comparisons default to stereo-pair analysis or allow individual
  channel selection?
- Should A and B use one shared reset epoch to guarantee matched integrated
  loudness windows?
- Is the current Octavia panel wide enough for another pair without harming
  legibility?
- Should the first implementation retain Goertzel analysis for compatibility,
  or move directly to a shared FFT backend?
