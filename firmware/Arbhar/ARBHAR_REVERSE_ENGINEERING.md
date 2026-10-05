# arbhar — firmware reverse engineering for a native Rack instrument

**Analyzed input:** `arbhar_updater_2-13.gz`, supplied by the user.  
**Analysis date:** October 5, 2026.  
**Release association:** 2.1.3; see the version qualification below.  
**Deliverable status:** an initial but substantial static reverse-engineering dossier, machine-readable evidence, and tested reference kernels. **Not a completed Rack module, a booted firmware image, or a hardware-equivalence claim.**

## 1. Executive findings

This update is exceptionally informative. It is an ordinary gzip-compressed tar archive containing **readable Pure Data patches, ARM/Linux shared libraries with useful symbols, configuration files, shell scripts, hardware-interface notes, and a 145 KB C control-source snapshot**. No encrypted payload or audio-update decoder is needed. The architecture resembles Lúbadh at the ARM/Linux platform level, but arbhar's application is primarily a **multi-process Pd graph surrounding custom DSP externals**, not the same application structure as Lúbadh. [F1–F4]

The most consequential findings for a native recreation are:

- **Two granular engines share six stereo layer stores.** The physical allocation is twelve arrays of 624,000 floats, not merely six ten-second mono buffers. At the requested 48 kHz rate, each channel holds thirteen seconds: a ten-second scan region plus additional storage relevant to grain tails and recording. [F3, S1]
- **The compiled player has 82 grain records per instance.** That does not establish 82 audible voices. Allocation, activity counting, and retirement logic must be reconciled with the manufacturer's advertised 88 sounding grains across two engines and an included design sketch that says 44. [S2, D1]
- **The timbral primitives are recoverable.** Exact saw/Gaussian/square templates, pitch and volume tables, a 101-by-515 generated window bank, a non-`tanh` rational clipper, and one four-point interpolation branch are documented and partly re-expressed in C++. [F5, S3–S5]
- **Length and Spray are not linear knobs.** The compiled control path squares normalized controls. Nominal grain duration is then scaled and clamped in the player; the central length value gives 750 ms at 48 kHz. The Scan and Follow placement paths are materially different. [S6]
- **Capture and onset behavior are central to identity.** Normal onset analysis uses a four-stage high-pass path and `bonk~`, not the tempting FFT test external also found in the archive. Recorder state includes two writer records and finite capture boundaries; it is not simply a continuously wrapping tape buffer. [F3, S7, D2]
- **The patch graph reveals effects routing, but some fidelity questions remain.** Delay scaling is not fully reconciled with configuration descriptions, and an auxiliary reverb branch is present without a proven nonzero normal startup gain. Neither should be silently converted into an assumed audible feature. [F3, S8]

For Rack, the recommended path is a native deterministic engine with an explicit control/event layer, rather than embedding an ARM emulator and hoping the rest of the operating environment disappears. The recovered graph and kernels make that path practical; the remaining bottleneck is **state/event fidelity**, not identifying whether this is a granular synthesizer. [R]

## 2. Evidence rules and scope

| Grade | Meaning |
|---|---|
| **F — file fact** | Directly present in the uploaded archive: bytes, names, source text, patches, presets, tables, or symbols. |
| **S — static reconstruction** | Derived from instructions or connected graph dataflow. It may not cover every branch or runtime condition. |
| **P — restricted numerical probe** | Selected actual disassembly text interpreted with a deliberately small finite-input instruction subset. Not a real ARM CPU execution. |
| **T — native test** | A check of the supplied C++ reconstruction, algebraic invariant, or fixture agreement. Not an original-instrument measurement. |
| **D — documentation** | Official published behavior, which may refer to a different revision or abstract away implementation details. |
| **R — recommendation** | A proposed Rack design decision, not a claim about original code. |

All hexadecimal function/data addresses in this dossier are **virtual addresses within the named ELF shared object**, not archive offsets and not global addresses shared between libraries. `evidence/elf/*/metadata.json` supplies ELF sections for translating virtual addresses to file offsets.

No manufacturer updater, shell script, hardware library, Pd patch, or complete firmware process was executed. Some scripts perform privileged writes, mount storage, change system configuration, or execute another script found on USB. They are included as **inert research evidence**. All extracted regular files were stored without executable permissions.

The available DWARF data is useful for the bundled **bcm2835 hardware library**. It is **not** recovered application/DSP structure debug information. The grain-record layout below was inferred from instructions, not read from a definitive C type.

The included `arbhar_gpio~.c_changes?` is a valuable but **unverified source snapshot**. Referenced headers including `arbhar_functions.h` and `arbhar_config.h` are absent. The source is a guide to concepts and names, not a substitute for checking the deployed binary. Similarly, `arbhar_structure.md` is a design sketch, not the compiled implementation.

### 2.1 Identity and package inventory

```text
Original archive SHA-256
1414f35d8d0e3ca8d4cb0871a8c61f3957001caf3b106d30df0514ca9f404637

arbhar_play~.pd_linux SHA-256
0ae8ae0ac13ae5952940d0deda17a44b76f1fcb16a40630f7ede3243ce09f8ae

arbhar_gpio~.pd_linux SHA-256
0ae07ca37403f805972cf88aedfd57296e689f4e93dbf1a60626adf09ee2be5f
```

The archive contains **184 regular files**. The analysis catalogues **19 ELF files**, **59 Pd patches**, **120 canvases**, **5,596 indexed graphical objects**, and **5,727 connections**. These include tests, utilities, and abandoned variants; they are **not all in the normal audio path**. Every parsed connection endpoint resolves to an indexed object, which checks parser consistency but does not prove correct Pd execution. [F1, F2, T1]

The supplied archive name and marker `arbhar_v2_2-13` are associated with the officially listed **2.1.3** release. The support page consulted on the analysis date lists 2.1.3 and describes USB/MIDI adjustments and a pitch-control fix. This is not a claim that the uploaded archive was hash-compared against a freshly downloaded official package. The full manual is labelled 2.0; the quickstart is labelled 2.1. [F1, D2–D4]

## 3. Platform, processes, and data ownership

### 3.1 Runtime launch sequence

`loadArbharV2.sh` identifies a Linux/Raspberry-Pi-style environment: `/home/pi`, `bcm2835` calls, GPIO commands, ALSA mixer restoration, CPU affinity, and Pd. The normal calibrated branch launches a calibration check, a non-audio file loader, the main audio patch, a separate controls/UI patch, and a background processing patch. The main audio command requests **48,000 Hz** and `-audiobuf 8`. The separate UI command requests `-audiobuf 25`; these are process options, not evidence that every DSP block has that many samples. [F3]

Inside `arbharMain.pd`, two `pd~` objects launch `arbharGrainCore.pd`. One has `-fifo 2`, the other `-fifo 12`; both expose four signal outputs and contain a custom player. Their roles correspond to externally struck and continuous grain production. Preserve the distinction between the two event sources and their output gains. Do not mistake Pd subprocess buffering options for the intended musical grain delay. [F3, S8]

A useful ownership model for a port is:

```text
calibrated controls / CV / buttons / MIDI
                  |
           event and mode logic
          /         |          
   Capture writer  Strike     Continuous
          |         engine      engine
          +---- six stereo layer stores ----+
                         |
                 audio/effects graph
                         |
                   stereo outputs

file conversion and scene loading: separate non-audio work
```

This is a functional summary, not an exact rendering of every Pd cable. The complete indexed netlists are supplied for exact graph inspection. [R; grounded in F3]

### 3.2 Shared memory and audio storage

The main control block is shared memory key **61019**, length **250** values. Audio stores use keys **90521 through 90532**, each with **624,000 float samples**. Allocation code uses System V shared-memory operations. The twelve channels together require **29,952,000 bytes**, about **28.56 MiB**, before other state, effects, import buffers, or undo copies. [F3, S1]

There are six stereo logical layers. At nominal 48 kHz, 624,000 samples is thirteen seconds, while normal position scaling spans 480,000 samples, or ten seconds. This is a strong reason not to allocate only ten seconds in a recreation. It is not permission to treat all thirteen seconds as the ordinary front-panel scan range. [S1, S6]

For a native Rack instrument, replace interprocess shared memory with typed in-process state. File workers can prepare complete staged layer sets; the audio thread should adopt prepared data at a defined event boundary without blocking, allocating, or destroying large buffers in its per-sample path. Those are port design choices, not properties inferred from the original shared-memory code. [R]

## 4. Grain engine and voice lifecycle

### 4.1 What the 82-slot finding actually proves

In `arbhar_play~.pd_linux`, `arbhar_play_tilde_new` at **0x619c** initializes a bank of records beginning at object offset **0x104**, with a stride of **0x74 = 116 bytes**. The initialization terminates after 82 records. `play_next` at **0x6c34** contains an index comparison against 81 and modulo-82 arithmetic; the perform routine walks the same stride. This is independently consistent evidence for an **82-record storage pool per player instance**. [S2]

The same `play_next` path checks an active-count threshold of **42** and can mark a candidate for retirement. The perform path reduces the retirement gain by **1/64 per perform-block visit**, then clears activity when appropriate. It is not a one-sample 1/64 fade. Existing MIDI/chord, override, transition, and allocation branches have not all been integrated into a complete semantic model. [S2]

Therefore:

- The included `NO_OF_GRAINS 44` sketch must not set the native storage pool solely because it looks like source.
- The compiled 82 must not be marketed as 82 simultaneous ordinary grains per engine.
- The documented 88 sounding grains across two engines remains an external behavior target, not a decoded allocation formula. [F4, S2, D1]

A sensible first implementation reserves enough records to preserve transitions and models activity separately from storage capacity. The exact retirement/stealing policy remains an explicit fidelity task. [R]

### 4.2 Partially recovered record fields

These are offsets **relative to one record**, not a guaranteed ABI-compatible C structure. Use them for continued analysis, not binary struct casting in a new plugin.

| Relative offset | Static interpretation | Qualification |
|---:|---|---|
| +0 | Record identifier | Initialization evidence. |
| +4 | Integer duration | Used by grain processing. |
| +8 | Start sample position | Placement/processing dataflow. |
| +20 | Read increment / speed | Playback dataflow. |
| +24 | Amplitude-related value | Full gain factorization remains incomplete. |
| +32 | Current playback progress | Needs full forward/reverse branch reconciliation. |
| +40 | Active flag | Allocation and perform agree. |
| +56 | Layer index | Layer-selection dataflow. |
| +60 | Direction | Placement writes positive/negative direction. |
| +64 | Texture/window-related control | Exact all-mode conversion remains open. |
| +68 | Window stepping state | Partial reconstruction. |
| +80 | Retirement gain | Multiplied/stepped during retirement. |
| +84 | Retirement flag | Separate from active flag. |

Additional offsets are present in the disassembly but intentionally left unnamed here rather than inventing a complete layout. [S2]

### 4.3 Interpolation and population gain

One inspected perform branch at **0x49f0–0x4a94** is a four-point interpolator. Its arithmetic is supplied as `cubic4`; the constant corresponding to one sixth is the stored float **0.16666670143604279**. The function passes DC and linear-ramp invariants. That does **not** settle wrap/clamp boundaries, the recording-head exclusion region, all reverse branches, or anti-alias behavior at large read increments. [S4, T2]

The player reads a recovered `VOL_REDUCTION` table using an activity-derived index clamped at **24**, then smooths the gain with an update of the form `old + 0.17 * (target - old)` at the perform-block level. A stereo-related adjustment uses `1 + 1.35 * (gain - 1)` in the inspected path. These are more specific than a generic `1/sqrt(N)` normalizer, but the exact definition of the counted population still needs to be connected to retirement and MIDI modes. [F5, S4]

## 5. Parameter mappings recovered from compiled code

### 5.1 Length and Spray

`_setPlayParameters` in the GPIO library, **0x7c50**, combines controls and relevant MIDI contributions, clamps a 12-bit domain, and squares normalized Length/Spray values before writing internal units:

```text
u = normalized calibrated control in [0, 1]
internal_units = truncate_float32(100000 * u²)

nominal_duration_samples = clamp(float(internal_units * 1.44), 128, 144000)
spray_samples            = float(internal_units * 4.8)
```

The binary uses specific float/double conversion sequences; the reference header preserves the identified scale constants. `durationSamplesFromControl(25000)` gives **36,000 samples**, or **750 ms at nominal 48 kHz**. The upper result is **144,000 samples**, or three seconds. The low-level clamp of **128 samples** is about **2.667 ms at 48 kHz**, whereas the manual describes an approximately four-millisecond minimum. Treat this as a low-level/internal versus user-facing range distinction that needs measurement, not as proof that one source is wrong. [S6, D3]

The recovered control writes are **SHM 101 for Length** and **SHM 103 for Spray**. Older notes assigning a different role to 103 are stale relative to this compiled path. MIDI terms at slots 209 and 212 are added before the clamp, using a factor of 32 in the inspected path. Native CV normalization must be specified separately from these raw hardware numbers. [S6]

### 5.2 Scan, Omega, and Spray placement

The ordinary Scan path uses approximately `480000/4095` sample scaling. Shared position units are then consumed by `_getPositionRange`, **0x6868**, with a 4.8 conversion. In Omega selection, a separate scale approximates `600000/4095`; a layer quotient is formed in units of 100,000 and position uses a modulo constant of **99,999**, not a silently idealized 100,000. The exact top endpoint can form a layer index of six and skip the ordinary valid-layer update. This is an important endpoint test, not an invitation to duplicate undefined behavior. [S6]

The inspected Scan/Spray branch draws an offset from an absolute/modulo expression and adds a nonnegative distance to an adjusted base. It is **not equivalent to casually implementing uniform ±Spray around the current scan position**. The Follow branch instead limits a backward-looking range relative to current position. Several edge/recording-boundary branches are not yet fully resolved; one comparison around **0x6a20** is specifically retained as an open dataflow question. The dossier does not claim a complete start-position algorithm. [S6]

### 5.3 Follow-speed curve

`calculateFollowSpeed`, GPIO VA **0x3f10**, is a self-contained function suitable for a restricted numerical probe. Let its 12-bit argument be `z`. It transforms the working coordinate as follows:

```text
mode 2: u = z
mode 0: u = 4095 - z
mode 1: u = 2 * (4095 - z)
```

Its algebraic response, ignoring tiny differences from the stored rounded constants, is:

| Working coordinate u | Speed |
|---|---|
| u < 1100 | `1 + 19 * (1 - u/1100)²` |
| 1100 ≤ u < 1300 | `+1` |
| 1300 ≤ u < 4095 | `1 - (u - 1300)/2795` |
| 4095 ≤ u ≤ 6890 | `-1 + (6890 - u)/2795` |
| 6890 < u ≤ 7090 | `-1` |
| u > 7090 | `-(1 + 19 * (1 - (8190 - u)/1100)²)` |

This exposes unity-speed plateaus, a zero crossing in the bidirectional profile, and accelerated quadratic tails. **“Inverted unidirectional” describes control direction, not an all-negative speed mode.** The helper's argument is not automatically the untouched physical ADC value. [S9, P1]

All **4,096 integer inputs in all three modes** were compared using an interpreter for the actual disassembly text. The 12,288 comparisons had no failures at tolerance `1e-5`; maximum absolute error from the independently expressed algebraic curve was about `2.18e-6`. Only this leaf routine received that original-instruction-text probe treatment. It does not validate the full Follow head, loop, or scheduling behavior. [P1]

## 6. Grain windows, pitch data, and nonlinearities

### 6.1 Exact tables and generated window bank

The main player contains named arrays `PITCH_VALUES`, `VOL_REDUCTION`, `SAW`, `GAUSS`, and `SQUARE`. The three envelope templates each contain **515 float32 values**. Their exact values, symbol addresses, and data hashes are recorded in `tables/extracted_float_tables.json`; a C++ header stores exact hexadecimal float literals. The 128-entry pitch table has unity at index 69. Do not infer from that alone that the complete pitch-control mapping is simply a MIDI note lookup. [F5]

`makingWndArray`, player VA **0x5b34**, fills a BSS object of **208,060 bytes**, exactly **101 × 515 × 4**. The constructor calls the generator. The static reconstruction uses row coordinate `k = 0.02 * (row - 50)` and an edge taper `T(i)` that rises over the first ten samples and falls over the last ten samples. [S3]

For the square-to-Gaussian half, the core expression is:

```text
max(0, (1 + k) * GAUSS[i] + 0.9 * (-k) * SQUARE[i + 1]) * T(i), k ≤ 0
```

For the Gaussian-to-saw half it is approximately:

```text
clamp(k * SAW[i] + (1 - k) * (1 + k) * GAUSS[j], 0, 1) * T(i), k > 0
j = min(515, truncate(i + 200*k))
```

The initial sample has a special zero path. The inspected binary layout supplies zero values at the relevant guard reads; the reference implementation explicitly supplies those zeros without an out-of-bounds C++ access. These windows are **not a simple convex crossfade between three stationary envelopes**: the Gaussian is shifted on one side, the blend includes `1-k²`, and the square endpoint includes a 0.9 gain factor. [S3]

`tables/reconstructed_window_bank.f32` is a **generated reconstruction**, not a directly extracted precomputed bank from the ELF. Complete texture-to-row selection and all grain-window phase/reverse behavior remain open. The reference `sampleWindowLinear` is clearly labelled a recommended lookup policy, not a recovered full perform implementation. [S3, R]

### 6.2 Rational soft clipping

`tanh_approx~.pd_linux`, perform VA **0x2268**, and the readable `hv_tanh~.pd` establish the core transfer:

```text
x = clamp(x, -6, 6)
y = gain * x * (27 + x*x) / (27 + 9*x*x)
```

At gain one, input six yields **14/13 ≈ 1.076923**, not one. Replacing this with `std::tanh`, or hard-clamping the result to ±1, changes the transfer. The graph uses this family of shaping at multiple locations, including delay write/read paths and final main clipping with a visible 0.89 multiplier. It is not adequately reproduced by placing an arbitrary saturation once at the output. [F3, S5]

The actual analog preamplifier, limiter, converter scaling, and output stage are not reconstructed by this digital formula. The native header adds finite-value guards as an explicit safety recommendation. [D1, R]

## 7. Capture, Dub, onset detection, and performance state

### 7.1 Recorder evidence

`arbharRecorder.pd` declares twelve 624,000-sample arrays and uses `arbhar_rec~` to write shared audio. The recorder perform routine at **0x4288** contains **two writer-state records with 48-byte stride**, used during capture handover. It also contains 256-sample fade counters and a Dub-related coefficient slew of about **0.0078 per sample**. [F3, S7]

These facts support implementing an explicit capture state machine with controlled transitions rather than an instantaneous overwrite operation. They do **not** yet supply a completely reconstructed Dub write equation, every fade phase, or every simultaneous onset/manual/accumulative transition. Those routines are in the address map and are priority follow-up targets. [S7]

The available evidence and official FAQ both argue against treating ordinary Capture as a perpetual ring recorder. Accumulative recording is a separate mode, with pause/resume and linkage configuration. Follow-loop behavior is playback behavior and must not be conflated with looping capture. [F3, D2]

### 7.2 Normal onset path

The connected recorder analysis path combines the analysis channels according to stereo state, high-passes the signal through **four `hip~ 300` stages**, and feeds:

```text
bonk~ -npts 256 -hop 32 -nfilters 11 -firstbin 0
```

The connected startup threshold message is `thresh 50 60`. Other threshold messages and FFT test patches exist, but presence in the archive does not make them the shipped normal analysis configuration. The list reaches the recorder's `onsetData` handling. `_processOnsetData` at **0x4044** processes sensitivity/hold state and shared flags; a hold-time path converts milliseconds using a factor of **48**. [F3, S7]

The exact installed `bonk~` implementation/version and its complete analysis internals are not included in the decoded custom DSP library. A recreation should identify and license an appropriate implementation or explicitly label a replacement detector. It should not claim the separate `fftwObject~` tester has revealed the original onset algorithm. [F2, R]

### 7.3 Configuration changes are musical behavior

Presets distinguish Capture CV latch/momentary/retrigger modes, Capture button semantics, onset profiles, Follow direction/loop options, modulation destination, probability controls, and quantisation sequences. The provided [parameter bible](PARAMETER_BIBLE.md) separates exact enum values from incomplete DSP laws. The [state-machine handoff](STATE_MACHINES.md) separates confirmed events from proposed software organization. [F6]

Track & Hold is a staged-parameter gesture, not a global audio freeze. The official quickstart describes holding the relevant gesture while changes are prepared and applying them on release. A port should stage only the parameter family defined by the mode, not suspend all CV, capture, or file state. Complete gesture timing and all button precedence still require a binary or hardware truth table. [D4, R]

## 8. Audio graph, effects, and mode boundaries

### 8.1 Mixing and input/output paths

`arbharMain.pd` is supplied both as original text and a resolved indexed graph. It contains input conditioning, recorder/analysis routing, two grain subprocesses, mode-dependent mixing, primary reverb, feedback delay, wavetable injection, clipping, muting, and stereo output handling. Normalizing this to “two grain engines into a reverb” would lose relevant gain and state behavior. [F3]

Visible main grain paths include **0.9** multipliers for the continuous path and startup **1.25** line-controlled factors in the struck path, with MIDI-related changes elsewhere. The manual's description of Strike as louder is not a proof that the complete audible ratio is simply `1.25/0.9`; other normalization and routing are involved. Keep this as a graph-level finding pending full gain tracing. [F3, D3]

The Dry/Wet steady-state law is sine/cosine-shaped: `dry = cos(pi/2 * wet)`, `wetGain = sin(pi/2 * wet)`. In the patch this is formed with zero-frequency oscillators and phase offsets, with additional low-pass smoothing. The supplied C++ pair captures the ideal law, not bit-identical Pd table interpolation or transient response. [F3, S8]

### 8.2 Primary reverb and auxiliary branch

The primary reverb is inspectable through `arbharReverb.pd` and `rev3adapt~.pd`; the `reducedReverb` subpatch exposes its processing network. The path includes a 0.85 input factor, cascaded low-pass stages at 12/10/9 kHz, high-pass conditioning at 150 Hz, and further post filtering. A saved 201-point control array is exported in `pd_saved_arrays.json`. Use the original graph for delay constants and all connections rather than substituting a generic “plate” based only on a name. [F3]

There is also an auxiliary `rev3~ 98 94 300 93` branch fed by filtered additional grain outputs. Its downstream gains are created at zero and no normal nonzero startup drive was established in this pass. **A wired branch is not proof of audible default behavior.** Keep it disabled in a conservative baseline until activation is traced. [S8]

Reverb interruption includes a fade to zero and a 250 ms restoration path. This supports ducking behavior but does not, by itself, prove that the reverb tank's delay memory is cleared. The official 2.1 notes specifically describe trigger ducking on Strike and/or onset. [F3, D2]

### 8.3 Feedback delay: useful evidence, unresolved macro law

`arbhar_feedback.pd` uses `block~ 16 1 2`, two `delwrite~` banks of 1,000 ms, four-point delay reads, rational shaping, and 12 kHz low-pass/60 Hz high-pass feedback conditioning. Delay time ramps over 50 ms, feedback ramps over 250 ms, and output level ramps over 150 ms. The inspected sign branches select feedback coefficients around **0.67** and **0.99**, with a near-center zero-feedback region. [F3, S8]

A key caution is the multiplier created as `* 0.125`: its right inlet is subsequently changed to **1** or **0.25** by sign-dependent messages. Reading the creation argument alone yields the wrong effective law. Likewise, the nominal delay capacity of 1,000 ms does not prove the front-panel macro directly spans 0–1,000 ms. [F3]

Under a simple steady-state reading of the local patch's incoming macro `m`, its intermediate timing expression is `m` for nonnegative values and `101 + 0.1*m` for negative values, with the dynamic multiplier above. That local reading does not straightforwardly agree with the explanatory preset text. Upstream MIDI offsets, message ordering, and the effective macro range need to be traced together before publishing a definitive knob-to-delay equation. The full graph is delivered; a speculative delay-control kernel is deliberately **not** presented as recovered code. [S8]

Effect order is configurable as parallel or series. The main patch gates dry-to-reverb, delay-to-reverb, and direct delay contribution according to shared slot 64. A native graph should use an explicit routing policy and test both transitions; changing routing must not accidentally double the dry signal or erase the effects state. [F3, R]

### 8.4 Wavetable mode and separate timebases

`arbharWavetable.pd` uses `arbhar_wtosc~ wt 7800`, a 259-element table, and `switch~ 64 1 1`. The control chain combines a centre-frequency pitch with raw pitch and V/oct-derived offsets, converts to frequency, and multiplies by **0.976642**. The shaping chain includes `tanh~ 0.3`, a 5 kHz low-pass, and `tanh~ 1.4`. Main Length thresholds and a clocked-mode condition control entry/mixing; the actual oscillator internals are only partially analyzed. [F3, S8]

Three timing facts must remain separate:

| Fact | What it does and does not establish |
|---|---|
| Pd launched with `-r 48000` | Requested audio rate, not a measured codec oscillator. |
| SoX raw file conversion uses **49148 Hz** | A file interchange convention visible in scripts. It is not automatically the live DSP rate. |
| Wavetable multiplier **0.976642** | A specific pitch correction in a specific graph. Its complete hardware rationale is unverified. |

Scripts trim raw conversion to thirteen seconds; at 49,148 Hz that is 638,924 frames, not 624,000. Buffer/file length handling must therefore be tested, not collapsed into one assumed sample rate. No clock constant from the earlier Lúbadh analysis has been imported as an arbhar fact. [F3]

## 9. Presets, startup, and hidden/experimental features

Seventeen preset/configuration texts were parsed, including six named factory presets and six numbered duplicate factory files. The matrix and full parsed values are included. The six factory profiles are Classic, Delay, Stereo, Follow Mode, Panning, and Accumulative Recording. [F6]

Notable distinctions include stereo input/phase settings, retrigger capture in the Follow preset, accumulative capture in the final preset, onset profile selection, modulation destination, and different quantisation sequences. Classic's **StrikeCVDelay is zero**, while the other five named factories specify **10 ms**. The initialization fallback also specifies **10 ms**. Do not implement “default preset” as an undifferentiated set of constants. [F6]

`checkForUserPreset.sh` gives startup precedence to a USB `preset.txt`, then the existing autosave preset, then `configurationDataInitFile.txt`. The fallback declares `LoadConfiguration 3`, whereas named factories declare 1. An HTML form's selected option and a file merely named `loadPresetOnStartup.txt` are not sufficient evidence of the actual startup configuration. [F3, F6]

Clocked-mode source, scripts, and editor variants are included. The initialization file explicitly has `EnableClockedModeSwitch 0` and `ClockedMode 0`, alongside BPM and multiplication/division tables. These are **present but disabled in that fallback**, not established default front-panel behavior. Treat clocked mode as a separate experimental compatibility phase until its activation conditions and shipped status are understood. [F6]

The quantisation arrays are ordered signed sequences, not just sets of pitch classes. Preserve duplicates and order. Random timing and random amplitude are independently configurable; the named factories enable the former and disable the latter. A host PRNG replacement should be acknowledged as a design choice until the original libc generator, seeding, and interprocess sequence relationships are reconstructed. [F5, F6, R]

## 10. Validation results and limitations

| Check performed | Result | Actual scope |
|---|---:|---|
| Extracted-file manifest verification | 184 files matched | Exact bytes and sizes against the manifest. |
| Pd netlist endpoint checks | 5,727 valid connections; zero invalid endpoints | Structural parse consistency only. |
| Restricted Follow instruction-text probe | 12,288 comparisons; zero failures | One leaf routine, finite integer inputs across three modes, tolerance `1e-5`. |
| Optimized native C++ test run | 107,421 checks; zero failures | Kernels, invariants, windows, and the Follow fixture. |
| ASan + UBSan native run | Same 107,421 checks passed | No sanitizer errors observed in those tests. |

The native count includes the Follow fixture comparisons; these counts are **not independent full-firmware validation totals**. No audio was compared against hardware, no original Pd performance graph was rendered, and no end-to-end CPU-emulated session was run. The window bank is statically reconstructed, while the Follow helper alone received the restricted original-instruction-text numerical probe. [T1, T2, P1]

## 11. What to implement now, and what to resolve first

The supplied kernels, layer geometry, two-engine organization, exact presets, connected effects patches, and control mappings are sufficient to begin a purposeful native implementation. They are not sufficient to call it a faithful completed reproduction. [R]

The highest-priority remaining work is **grain launch/retirement and scheduler timing**, then **recording/Dub handover and boundary rules**, followed by **full delay macro scaling and gain routing**. Pitch deviation/quantisation selection, texture phase mapping, stereo panning, exact onset behavior, and wavetable entry are additional explicit compatibility tasks. Each has a starting function or patch in the [validation plan](VALIDATION_PLAN.md). [R]

An ARM emulator could be valuable as an offline differential oracle for isolated routines, or with substantial work as part of a Pd/OS compatibility harness. An ARM core alone would not supply Pd APIs, its message scheduler, child processes, shared memory, ALSA, hardware I/O, or the appropriate external libraries. For a distributable Rack design, avoid making the audio thread responsible for this entire environment. [F3, R]

## 12. Evidence navigation and sources

### Uploaded-byte evidence

**F1 — Package identity:** `evidence/manifest.json`; verifier `tools/verify_manifest.py`; safe extractor `tools/safe_extract.py`.

**F2 — Executables:** `tables/binary_inventory.json`, `tables/symbols.csv`, and `evidence/elf/<binary>/readelf.txt`, `metadata.json`, `disassembly.txt`. `annotated/` adds literal/import hints; hints are aids, not a decompiler or complete pointer analysis.

**F3 — Graph/scripts:** `extracted/loadArbharV2.sh`, `arbharMain.pd`, `arbharGrainCore.pd`, `arbharRecorder.pd`, `arbharReverb.pd`, `rev3adapt~.pd`, `arbhar_feedback.pd`, `arbharWavetable.pd`, `checkForUserPreset.sh`, and conversion scripts. Machine views: `tables/pd_netlists.json`, `evidence/main_graph.txt`, `evidence/feedback_graph.txt`, `tables/shmem_patch_references.json`.

**F4 — Non-authoritative source aids:** `extracted/arbhar_gpio~.c_changes?`, `extracted/arbhar_structure.md`, `extracted/Pin_Adc_Control_Info/`.

**F5 — Tables:** `tables/extracted_float_tables.json`, `tables/pd_saved_arrays.json`, `reference/recovered_tables.hpp`.

**F6 — Configuration:** `extracted/factoryPresets/`, `_factoryPresets/`, `configurationDataInitFile.txt`, `arbhar_Preset_Editor.html`; `tables/presets.json`, `preset_matrix.csv`, `preset_editor_fields.json`.

### Static reconstruction anchors

| ID | Binary or patch | Main anchors |
|---|---|---|
| S1 | `arbhar_play~.pd_linux`, recorder/loader patches | `_memAllocate` 0x6074; 624000-sample arrays and shared keys. |
| S2 | `arbhar_play~.pd_linux` | Constructor 0x619c; `play_next` 0x6c34; perform 0x4248; retirement region 0x4674–0x46a8. |
| S3 | `arbhar_play~.pd_linux` | `makingWndArray` 0x5b34 and named template arrays. |
| S4 | `arbhar_play~.pd_linux` | Perform 0x4248; interpolation region 0x49f0–0x4a94; `VOL_REDUCTION`. |
| S5 | `tanh_approx~.pd_linux` | Perform 0x2268; cross-check `hv_tanh~.pd`. |
| S6 | GPIO + player | `_setPlayParameters` 0x7c50; `_getPositionRange` 0x6868. |
| S7 | `arbhar_rec~.pd_linux` | `_processOnsetData` 0x4044; perform 0x4288; recorder patch. |
| S8 | Main/effects/wavetable Pd patches | Exact object/edge records in `tables/pd_netlists.json`. |
| S9 | `arbhar_gpio~.pd_linux` | `calculateFollowSpeed` 0x3f10. |

**P1:** `tools/probe_follow.py`, `tables/follow_probe.csv`, `tables/follow_probe_results.json`.

**T1:** `tables/structural_validation.json`; `tools/build_structured_data.py`.

**T2:** `reference/`, `tables/native_test_results.json`, `tables/native_sanitizer_test_results.json`. See `reference/README.md` for commands and boundaries.

### Official external references

Consulted October 5, 2026; no manufacturer ownership or licensing rights are transferred by including references or extracted research evidence.

- **D1:** [Instruō arbhar product page](https://www.instruomodular.com/product/arbhar/): public feature targets and engine/buffer descriptions.
- **D2:** [Instruō firmware/support page](https://www.instruomodular.com/firmware/): release labels, 2.1/2.1.3 notes, and no-ring-capture FAQ.
- **D3:** [arbhar 2.0 manual](https://www.instruomodular.com/wp-content/uploads/2024/01/Arbhar-Manual-Firmware-2.0-web.pdf): selected control, grain, and panel pages; relevant page images inspected. Not a claim that every manual page was exhaustively reconciled with 2.1.3.
- **D4:** [arbhar 2.1 quickstart](https://www.instruomodular.com/wp-content/uploads/2025/03/arbhar-Quickstart-Firmware-2.1.pdf): newer performance/configuration behavior; relevant page images inspected.
- **D5:** [VCV Rack voltage standards](https://vcvrack.com/manual/VoltageStandards): host-interface recommendations, not measurements of arbhar hardware.
