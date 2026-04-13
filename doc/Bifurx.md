# Bifurx (Leviathan): Belgrad-Inspired Dual-Peak Multimode SVF for VCV Rack With Real-Time Curve + Spectrum Preview

## What the manual specifies about interface and behavior

The hardware module described in the attached manual is an all‑analog “dual peak multimode state variable filter” built around **two** filter cores whose “inputs and outputs can be combined in various serial and parallel configurations,” with a **mode rotary** selecting “one of ten unique filter responses.” The manual emphasizes: dual-core design, “10 distinct frequency responses,” a wide tuning range (“approx. 4Hz to 28kHz”), voltage control over resonance and “peaks balance,” plus “nonlinear feedback and cross‑modulation,” a V/oct input, and “adjustable input overdrive.”

The front‑panel control intent (which we will mirror in VCV Rack) can be summarized as:

- **LEVEL**: Controls amplitude into the filter “up to some degree of input overdrive.”  
- **FREQ** (large knob): Sets the center frequency of both cores (~4 Hz–28 kHz). Frequency is CV‑controlled via **V/OCT** and modulated by **FM**, with FM depth set by an illuminated slider.  
- **SPAN**: Symmetric detuning between the two cores around the center cutoff, enabling formant/vocal-like shapes; SPAN is described as “deliberately non-linear.”  
  - SPAN CV is noted as bipolar (−5 V to +5 V) and scaled by an illuminated attenuator.  
- **RESO** + RESO CV: Overall resonance, with RESO CV specified as 0–8 V.  
- **BALANCE** + BALANCE CV: Skews the two resonant peaks from lower‑peak dominance, through symmetry, to higher‑peak dominance; BALANCE CV is bipolar (−5 V to +5 V).  
- **TITO** 3‑position switch: A coupling topology that applies “double nonlinear audio‑rate modulation” to the cores; neutral → clean, up → self‑modulation (“sm”), down → cross‑modulation (“xm”). The manual characterizes SM as more radical/textured, XM as warbly/chirpy.

Self‑oscillation is a first‑class behavior: with RESO around ~8 (mode/setting dependent), it self‑oscillates and behaves like one or two sine VCOs controlled by V/OCT, with an expectation of ~5 octaves of tracking when tuned carefully.

The manual’s “Technical specification” section gives us the target electrical ranges and panel constraints we’ll reproduce at the I/O layer:

- Width: 14 hp  
- Audio I/O nominal capability: input 0–20 Vpp (recommended 10 Vpp), output 0–20 Vpp  
- CV ranges: V/OCT −10…+10 V, RESO 0…+8 V, BALANCE −5…+5 V, FM −10…+10 V, SPAN −10…+10 V

Finally, the manual includes explicit copyright restrictions on copying/distribution/commercial use without written permission. That matters because a “clone” in software should avoid reusing protected panel art, plots, and textual phrasing verbatim, even if functional behavior is emulated. In practice, we can mirror control semantics and port topology while producing original UI assets and original explanatory text.

## Scope and requirements for the VCV Rack module

This section treats your request as an implementable spec for a code assistant (Codex-style), while staying faithful to the manual’s I/O semantics and the front‑panel workflow.

Module naming for the Leviathan plugin family:

- Module name: **Bifurx**

Core goals (first release, mono):

- A **mono** audio module that mirrors the manual’s control surface: MODE (10 positions), LEVEL, FREQ, RESO, SPAN, BALANCE, FM amount slider, SPAN CV attenuator slider, and a 3‑state TITO switch, with the same jack set (INPUT, OUTPUT, V/OCT, FM, RESO CV, BALANCE CV, SPAN CV).  
- A **dual‑core SVF-based** DSP structure matching the manual’s description (serial/parallel recombination under the 10 mode settings).  
- A major divergence: an embedded **preview window** that renders:
  - the **current filter magnitude response curve** (“actual filter curves”), and
  - an underneath “spectrum modification” visualization, colored **purple for attenuation**, **cyan for boost**, and a **white/near‑white** region for near‑neutral response (with smooth gradients).

Non-goals for v1:

- True stereo processing. You explicitly accept mono for the first release.

Planned later (not in v1, but design for it now):

- A “stereo via two monos” workflow using an **expander‑style parameter sync** mechanism (master pushes settings; follower mirrors). This should be forward-compatible with Rack’s expander architecture, which uses adjacent-module message passing with a double-buffer scheme and 1-sample message latency.

## DSP architecture that can plausibly reproduce the ten responses

### Why a TPT state-variable core is the right baseline

Belgrad’s core claim is two state-variable filter cores plus nonlinear/cross modulation. For a digital module, a “topology-preserving transform” (TPT) SVF is a strong choice because it was developed specifically to preserve topology under parameter modulation and to accommodate nonlinearities more naturally than “transfer-function then biquad” workflows.

Concretely: Zavalishin describes that classic bilinear-transform transfer-function design can produce “strong undesired artifacts” under high-rate modulation and makes modeling nonlinearities of the original topology difficult, motivating topology-preserving approaches. This aligns with Belgrad’s emphasis on audio-rate modulation/coupling via the TITO switch.

### Proposed signal flow primitives

We’ll implement two identical SVF “cores,” each producing at least these outputs per sample:

- LP (lowpass), BP (bandpass), HP (highpass), and NOTCH (bandstop = LP + HP).

Per the manual, the two cores are tuned around a **center** frequency (FREQ), then **symmetrically detuned** by SPAN to create the two resonant peaks. A practical mapping is to treat SPAN as a distance in octaves and apply it symmetrically in log-frequency:

- `f1 = f0 * 2^(-spanOct/2)`  
- `f2 = f0 * 2^(+spanOct/2)`

This respects “symmetric detune” and makes “nearly eight octaves” straightforward: `spanOct ∈ [0, ~8]`. The manual also says the SPAN knob is “deliberately non-linear,” so the knob taper should be shaped (e.g., power curve) rather than linear in octaves.

Resonance and peak balance:

- RESO sets the overall feedback/Q behavior and supports self‑oscillation around ~8.  
- BALANCE skews which peak dominates: lower vs higher peak control.  

A digital approximation that matches the described “domination” behavior is to derive per‑core resonance factors from a shared “resoAmount” and a signed “balance” parameter. Example approach:

- `reso1 = resoAmount * exp(-k * balance)`  
- `reso2 = resoAmount * exp(+k * balance)`

This yields symmetric behavior around center (balance=0), with progressively stronger emphasis on one core’s peak as balance deviates.

Nonlinearities and TITO coupling:

- The manual describes “nonlinearities inherent in the feedback paths” as resonance increases, plus a TITO-controlled “double nonlinear audio-rate modulation” topology offering self-mod and cross-mod.  
- A feasible DSP surrogate is:
  - apply a soft-saturator (e.g., `tanh`-like) in the resonance feedback path, and
  - apply audio-rate modulation of internal SVF parameters (or input) using BP signals, with routing depending on TITO state.

This is conceptually consistent with using topology-preserving / time-varying approaches that keep nonlinear options tractable.

## Mode mapping that mirrors the manual’s ten responses

The manual’s ten modes describe the *qualitative topology* of how the two cores are combined (cascade vs parallel mix, and which outputs are used). The design below is an implementable mapping that aims to reproduce those qualitative response families. It won’t be a component-perfect analog model (we don’t have the schematic), but it is faithful to the described filter math and mode intent.

In the following, Core A is tuned to `f1` (lower), Core B to `f2` (higher). “Cascade” means Core B processes Core A’s output.

**Double slope lowpass (LL)**  
Manual: near-flat lows; −12 dB/oct between peaks; −24 dB/oct above second peak; SPAN=0 acts like classic 4‑pole LP, SPAN max tends toward 2‑pole LP.  
Implementation: cascade two 2‑pole lowpasses: `y = LP_B( LP_A(x) )`.  
Rationale: two separated cutoffs yield the “double slope” (one pole pair acts first, the other later), and equal cutoffs yield 4‑pole behavior.

**Lowpass + bandpass (LB)**  
Manual: 2‑pole lowpass character, second core in bandpass adds higher‑frequency formant bump; mixing produces “phase cancellation” and an audible gap; above higher peak response is −6 dB/oct.  
Implementation: parallel mix of `LP_A(x)` with a signed amount of `BP_B(x)` (and optionally a small `HP_B(x)` term) to craft the gap region.  
Notes: The “gap” can be achieved by subtractive mixing around mid-band; exact cancellation will depend on SVF phase response and chosen weights.

**Lowpass with notch in passband (NL)**  
Manual: −12 dB lowpass overall; one core creates a notch below the LP corner.  
Implementation: `y = LP_B(x) - k * BP_A(x)` where Core A’s BP is tuned below the main cutoff (via SPAN), producing a subtractive “dip” in the passband.

**Double notch (NN)**  
Manual: cascaded connection of two 2‑pole notch filters; with resonance down it resembles a short phaser; notch spacing set by SPAN.  
Implementation: `y = NOTCH_B( NOTCH_A(x) )`, with NOTCH = LP + HP from each SVF core.

**Lowpass + highpass (LH)**  
Manual: variable-width band‑rejection with resonant peaks at stopband corners; mostly bottom and top audible.  
Implementation: parallel sum: `y = LP_A(x) + HP_B(x)` (with per‑mode gain trim).  
The stopband is naturally formed because LP suppresses highs and HP suppresses lows, leaving extremes. Resonant peaks come from high Q near each corner.

**Double bandpass (BB)**  
Manual: two peaks mixed, creating two‑formant response; peak bandwidth depends on RESO and BALANCE.  
Implementation: `y = w1 * BP_A(x) + w2 * BP_B(x)` with weights derived from BALANCE.

**Bandpass (HH)**  
Manual: cascade a 12 dB/oct highpass and 12 dB/oct lowpass with two resonant peaks at passband ends; at higher SPAN response between peaks becomes near-flat.  
Implementation: `y = LP_B( HP_A(x) )`. With larger spacing between cutoffs (SPAN high), mid-band becomes flatter.

**Highpass with notch in passband (HN)**  
Manual: 12 dB highpass overall; notch above the HP corner created by the other core.  
Implementation: mirror NL: `y = HP_A(x) - k * BP_B(x)` where Core B is tuned above the HP corner.

**Bandpass + highpass (BH)**  
Manual: 2‑pole highpass character, first core in bandpass adds lower formant; mixing yields gap; below lower peak response is +6 dB/oct.  
Implementation: parallel mix `HP_B(x)` with signed `BP_A(x)` (plus optional LP compensation) to produce the lower bump and cancellation gap.

**Double slope highpass (HL)**  
Manual: near-flat highs; +12 dB/oct between peaks; +24 dB/oct below first peak; SPAN=0 behaves like classic 4‑pole HP, SPAN max tends toward 2‑pole HP.  
Implementation: cascade two 2‑pole highpasses: `y = HP_B( HP_A(x) )`.

Because the manual warns that modes naturally differ in gain and resonance behavior (e.g., narrow bandpass vs narrow notch), we should not “auto-normalize” everything by default—unless you want an optional quality-of-life menu item.

## Preview window: curve rendering plus “spectrum modification” coloration

This is the major divergence you requested: a real-time, explanatory visualization that makes the filter’s action legible at a glance.

### Rendering the “actual filter curve”

We will compute and draw a magnitude response curve over a log-frequency axis similar to the manual’s plot style (dB vs Hz, log x-axis). The manual’s mode plots show a frequency axis with decades (e.g., 10, 100, 1k, 10k) and a dB range extending down to around −80 dB.

A pragmatic approach:

- Define `Ncurve ≈ 256–512` sample points `fi` log-spaced from ~10 Hz to Nyquist (or capped at 20 kHz visually), with a clamped minimum/maximum so the curve remains stable across sample rates.  
- For each mode, compute the complex transfer `H(fi)` of the *linearized* digital topology (ignoring saturators and TITO audio-rate modulation). This is important: classic transfer functions apply cleanly to linear time-invariant behavior; Belgrad’s “double nonlinear audio-rate modulation” is intentionally non-LTI, so the curve is best treated as a “nominal” reference.  
- Convert to magnitude dB: `magDb = 20*log10(|H(fi)| + eps)` and draw the polyline.

Implementation detail: reuse biquad transfer math for display  
Even if the audio engine uses TPT SVFs, for the **display** it is simpler and stable to compute each 2‑pole section with standard biquad coefficients and evaluate `H(e^{jw})` directly. The W3C note “Audio EQ Cookbook” (adapted from Robert Bristow-Johnson) provides canonical biquad coefficient formulas for lowpass/highpass/bandpass/notch types using bilinear transform, including the normalized transfer form.  
This makes it straightforward to build per-mode display transfer functions as cascade products and parallel sums of biquads, matching the mode topology described above.

### Computing the colored “spectrum modification” layer

Your goal statement: “a colored spectrum graph under the curve that shows how frequencies are being modified by the filter,” with purple for decreases and cyan for increases, blending through white near neutral.

A faithful, robust interpretation is to compare **short-time spectra of input vs output** and derive a per-frequency gain estimate:

1. Maintain a short rolling window (e.g., 1024–4096 samples) of:
   - the signal entering the filter (`x[n]` after LEVEL drive stage, before filtering), and
   - the signal leaving the filter (`y[n]` after filtering and any final trim).
2. Compute real FFT magnitudes `|X[k]|`, `|Y[k]|` periodically (e.g., 20–30 fps).
3. Estimate per-bin gain ratio `G[k] = |Y[k]| / (|X[k]| + ε)` and convert to dB difference `D[k] = 20 log10(G[k])`.
4. Map `D[k]` to color:
   - `D < 0`: purple intensity increases with |D|  
   - `D ≈ 0`: white  
   - `D > 0`: cyan intensity increases with D  
   Use smooth interpolation (e.g., linear in dB across a clamp range, like ±24 dB).
5. Draw a filled area (or vertical bars) under the response curve, using alpha that depends on input energy (bins with near-zero `|X[k]|` should fade toward neutral to avoid unstable ratios).

Rack-side FFT mechanics  
Rack provides `rack::dsp::RealFFT`, a wrapper around PFFFT, with important constraints: FFT length must be a multiple of 32, and buffers must be 16-byte aligned (which ordinary allocations satisfy). This is ideal for 1024/2048/4096 point FFT sizes.

Threading and buffering (don’t stall the audio engine)  
To avoid locking on the audio thread, use a single-producer/single-consumer ring buffer: Rack’s `rack::dsp::RingBuffer` is explicitly a lock-free queue “with fixed size and no allocations” and supports a single producer and consumer, which matches “audio thread writes, UI thread reads.”  
We should not use `DoubleRingBuffer` for cross-thread communication without extra care, since its docs state it is “not thread-safe.”

UI redraw performance  
Because Rack calls `Widget::draw()` every frame and complex NanoVG drawing can be expensive, Rack’s plugin guide recommends caching complex widgets inside a `FramebufferWidget` and marking it dirty only when redraw is needed.  
For the preview window, the intended flow is:
- compute FFT + curve data at ~30 fps (or less),
- update cached vertex/color arrays,
- call `framebuffer->setDirty()` once per update.

This gives you rich visuals without murdering frame time.

## VCV Rack implementation design for Codex-style code generation

### Module skeleton and UI drawing hooks

Rack’s widget API centers on overriding `draw(const DrawArgs&)` and (optionally) `drawLayer(const DrawArgs&, int)` for multi-layer drawing. ModuleWidget’s docs emphasize that overriding draw should call the superclass to recurse to children.

We will implement:

- `BifurxModule : rack::engine::Module`
- `BifurxWidget : rack::app::ModuleWidget`
- `ResponseDisplay : rack::widget::Widget` (or TransparentWidget-like) nested inside a `FramebufferWidget`

We will use an SVG panel as the base (standard Rack approach), via `SvgWidget`/ThemedSvgPanel patterns.

### Parameters, inputs, outputs, ranges

Expose the same conceptual ports and controls as the manual, preserving CV range expectations and semantics:

- Inputs: IN, V/OCT, FM, RESO CV, BALANCE CV, SPAN CV  
- Outputs: OUT  
- Parameters: MODE (10-step), LEVEL, FREQ, RESO, BALANCE, SPAN, FM_AMT slider, SPAN_CV_ATTEN slider, TITO (3-step switch)

CV scaling guidelines (codex-ready, consistent with the manual’s published ranges):

- V/OCT: treat as 1 V/oct exponential pitch modulation, accept −10..+10 V.  
- RESO CV: 0..8 V mapped into the resonance range; clamp negative CV to 0 unless you want “negative resonance” as an intentional feature (not specified).  
- BALANCE CV: −5..+5 V mapped to balance domain (e.g., −1..+1).  
- FM CV: accept −10..+10 V with depth controlled by the FM slider.  
- SPAN CV: accept −10..+10 V (tech spec) but scale so that −5..+5 V covers musically “full range” if you want to respect the earlier textual mention.

Audio amplitude expectations:
- The manual describes 10 Vpp recommended input level and up to 20 Vpp capability. In Rack terms, this corresponds well to common ±5 V to ±10 V modular conventions, but we treat these as guidance: implement LEVEL gain such that typical Rack audio (±5 V) can drive into saturation at upper LEVEL settings.

### Expander-ready parameter sync design (future)

Rack expander messaging uses the `Module::Expander` double-buffer mechanism. Official docs show that an expander consumer should allocate both producer and consumer message buffers, must check the expander model before writing, writes to producerMessage, then requests a flip and reads from consumerMessage with 1-sample latency.

Forward-compatible design choice:
- Define a packed struct containing *all normalized parameters* needed to mirror the module: mode index, freq (normalized), reso, balance, span, FM slider, span-atten slider, tito state.
- Master module publishes this every sample (or via a low-rate divider) to its rightExpander.
- A future “follower” module reads and applies these values, optionally locking out local panel edits when linked.

This achieves your “two modules act as pseudo-stereo pair” idea with minimal DSP changes later.

### Notes on polyphony

Even though you want mono behavior, Rack supports polyphonic cables, and the plugin guide documents a standard approach: treat DSP states as arrays and loop over channels.  
A sensible v1 compromise:
- Process channel 0 only and output mono, or
- Process all channels independently (still “mono module,” but poly-capable in Rack terms).  
If you want the most future-proof implementation, the second option is usually kinder to users and aligns with Rack norms.

### Testing and calibration expectations

Behavioral tests derived from the manual:

- **Mode sweep sanity**: switching MODE should traverse responses “gradually change from lowpass to highpass,” including intermediate exotic responses, and it is normal for levels to change between modes.  
- **Span behavior**: SPAN=0 should collapse dual-peak modes toward the corresponding “single cutoff” archetype (e.g., 4‑pole behavior in LL/HL).  
- **Self oscillation**: with no input and RESO around the threshold, the module should self-oscillate (mode dependent) and respond musically to V/OCT with ~5 octaves of reasonably stable tracking.  
- **TITO character change**: switching TITO from neutral to SM/XM should audibly alter resonance texture (SM more radical/textured, XM more warbly/chirpy).  
- **Preview validity**: the curve should respond instantly to parameter changes; the spectrum coloration should show purple where energy is being reduced and cyan where increased, fading to neutral where the input has no energy.

Performance tests derived from Rack docs:

- Confirm the preview does not trigger draw-heavy CPU spikes by using `FramebufferWidget` redraw caching and limiting updates.  
- Confirm FFT sizes satisfy `RealFFT` constraints (multiple-of-32 length).  
- Confirm ring-buffer usage respects single producer/consumer guarantees (audio thread vs UI thread).

Legal/asset hygiene:
- Use original panel SVG art and original plots; do not ship screenshots of the manual’s mode graphs, since the manual asserts restrictions on copying/distribution/commercial use without permission.  
- Consider naming/branding differences from Xaoc Devices to avoid confusion; functional compatibility and inspiration can coexist with distinct identity.
