# Module Database & Operations Tables — VCV Rack Reference

Load this file for: choosing modules by task, troubleshooting matrix, CPU optimization.

### Module Database — Recommended by Category

**Oscillators:**
- VCV VCO-1 / WT VCO — free, reliable, core
- Audible Instruments Plaits (Macro Oscillator 2) — 24 modes, includes FM, granular, Karplus
- Bogaudio VCO / XCO — precise, analog-character
- SurgeXT VCO — wide range, wavetable, FM

**Filters:**
- VCV VCF — Moog Ladder style, 24dB LP
- Vult Tangents / Freak — multiple filter types, drive
- Audible Instruments Ripples — smooth, musical
- Bogaudio LVCF / VCF — versatile, CPU-light

**VCA / Envelopes:**
- VCV VCA + ADSR — baseline, always works
- Befaco Rampage — dual slope generator, function generator, LPG mode
- Bogaudio DADSR — delay + ADSR, full control

**Sequencers:**
- Impromptu PhraseSeq16/32 — most capable, chord sequences, per-step gates
- Impromptu Gate-Seq-64 — 64-step TR-style drum sequencer
- Count Modula — Euclidean, boolean logic, switches

**Mixers:**
- **MindMeld MixMaster** — de-facto professional standard: 16 tracks, 4 aux, EQ, compressor, true stereo, LUFS meter, mute groups. Use this for any serious mix.
- Bogaudio MIX4 / MIX8 — lightweight, good for submixes

**Reverb / Delay:**
- Valley Plateau — lush hall reverb, size/decay/pre-delay/damping
- Chronoblob2 — tempo-sync delay, ping-pong, feedback filter

**Generative / Random:**
- Audible Instruments Random Sampler (Marbles) — Turing Machine, quantized random, gates+CV
- HetrickCV — logic, chaos, probability utilities

**Utility:**
- Bogaudio 8VERT / OFFSET / SLEW / BOOL — essential toolkit
- Stoermelder STRIP / MIDI-CAT — patch documentation, MIDI mapping
- Impromptu Clocked — master clock, PPQN, swing, reset

**Analysis:**
- VCV Scope — oscilloscope, XY, FFT
- NYSTHI Multimeter — RMS, peak, frequency

---

### Module Quick-Pick by Task (Agent Cheat Sheet)

Use this when selecting which module to add for a given role. Ordered by preference.

| Task | 1st Choice | 2nd Choice | 3rd Choice | Poly? |
|---|---|---|---|---|
| VCO analog warm | Bogaudio VCO / XCO | VCV VCO-1 | Instruo Tš-L | yes |
| VCO FM TZ | Bogaudio FM-OP | NYSTHI TZOP | Surge XT | yes |
| VCO Wavetable | VCV WT VCO | Surge XT | Plaits WT Mode | yes |
| VCF clean | Vult Freak | Audible Ripples | Bogaudio VCF | yes |
| VCF character | Vult Tangents | Valley Feline | Bogaudio LVCF | partial |
| VCA | VCV VCA | Bogaudio VCA/VCMIX | Audible Veils | yes |
| ADSR | VCV ADSR | Bogaudio ADSR/DADSR | Count Modula EG | yes |
| LFO | Bogaudio LFO/4FO | Frozen Wasteland LFO | VCV LFO-2 | yes |
| Random / S&H | VCV RANDOM | HetrickCV Dust | Audible Marbles | yes |
| Quantizer | ML Quantum | VCV Quantizer | Bogaudio ADDR-SEQ | yes |
| Sequencer melodic | Impromptu PhraseSeq32 | JW NoteSeq | Voxglitch Digital Seq | yes |
| Euclidean drums | Count Modula Euclidean | Frozen Wasteland Seeds | Impromptu GateSeq64 | yes |
| Mixer pro | MindMeld MixMaster | Bogaudio MIX16 | VCV MIXER | yes stereo |
| EQ | MindMeld EqMaster | Bogaudio PEQ14 | — | yes |
| Compressor | Vult Comp | MindMeld Track Comp | VCV Compressor | stereo |
| Reverb | Valley Plateau | Valley Interzone | VCV Reverb | stereo |
| Delay | Chronoblob2 | VCV Delay | Valley Interzone | stereo |
| Distortion / Sat | Vult Debriatus | Vult Freak Drive | Befaco Chopping Kinky | yes |
| Granular | Audible Clouds | Voxglitch Grain Engine | NYSTHI Simpliciter | stereo |
| Sampler | NYSTHI Simpliciter | Voxglitch WavBank | — | stereo |
| MIDI MPE | moDllz MIDIpolyMPE | VCV MIDI-CV MPE Mode | — | P16 |
| Controller map | Stoermelder MIDI-CAT | VCV MIDI-MAP | — | — |
| Scope / Tuner | VCV Scope | NYSTHI Multimeter | NYSTHI TUNATHOR | — |
| Utility math | Bogaudio OFFSET/SUM/BOOL | VCV SUM/8VERT | Submarine/Venom | yes |
| Physical modeling | Audible Rings/Elements | — | — | yes |
| Drum synth | Vult Trummor 2 | Bidoo dTrOY | — | partial |
| FM TZ vintage | NYSTHI TZOP + DX7 Env | Bogaudio FM-OP ×4 | Surge XT | partial |
| Feedback drone | ComfortZone TZFM Lead | Bogaudio FM-OP + loop | — | — |

**Voltage quick-reference when connecting:**
- Audio: ±5V nominal (±10V = 0dBFS clip)
- CV unipolar: 0–10V | CV bipolar: ±5V
- Gate/Trigger: 10V, 1ms pulse
- Pitch: 1V/oct, 0V = C4 = 261.63Hz

---

### Troubleshooting Matrix

| Symptom | Most likely cause | Fix |
|---|---|---|
| No sound at output | VCA closed, gate missing, Audio module wrong | Check gate→ADSR→VCA chain; set VCA CV to 10V manually to test |
| Silence only on some notes | ADSR Release too short + notes overlap | Increase Release; check gate polarity |
| Out of tune / wrong pitch | VCO fine-tune off, or 1V/oct offset | Right-click VCO → Initialize; verify C4 = 0V = 261.63 Hz |
| Clicks/pops on note start | ADSR Attack = 0 | Set Attack ≥ 1.5 ms (≥ 0.01 on 0–1 scale) |
| Clicks on note end | Release = 0, abrupt VCA cutoff | Set Release ≥ 5 ms |
| Hum / 50 Hz buzz | DC offset, ground loop | HPF at 30 Hz before Audio module; check interface |
| Phase cancellation (thin sound) | Two copies of same signal summed with opposite phase | Invert one signal via attenuverter at -100% and compare |
| Bass disappears in stereo | Sub-bass content is out of phase between L/R | Force bass to mono (HPF on Side channel at 150 Hz) |
| Clipping at Audio module | Signal > ±10V | Add limiter (Ceiling ±9.5V) before Audio output |
| CPU dropouts | Too many polyphonic voices, expensive reverbs | Reduce poly count; increase blocksize to 512; bounce Plateau to audio |
| Sequencer drifting / off-beat | Clock/Reset race condition | Use Clocked module; ensure Reset fires 1 ms before Clock after Reset |
| Aliasing artifacts | High-frequency digital oscillators without anti-aliasing | Use bandlimited oscillators; increase sample rate to 96 kHz |
| Turing Machine always the same | Probability at 0% (locked) | Increase to 50–85% for evolution |
| Turing Machine pure noise | Probability at 100% | Reduce to 85%; always use Quantizer downstream |
| Quantizer ignores CV | Scale mask not set, or wrong scale selected | Right-click Quantizer → set scale; check CV range is 0–10V |
| LFO not modulating | Attenuator at 0 downstream, or CV sum out of range | Check attenuverter settings; LFO output ±5V needs attenuating |
| Reverb tail too muddy | No HPF on reverb send or return | Add HPF 200–300 Hz on reverb input; LPF 8 kHz on return |
| CPU dropout during MPE | Too many voices + expensive modules | Reduce to 8 voices live; move Reverb/Granular to Send bus; increase blocksize to 512 |
| Saturation adds DC offset | Heavy drive without HPF | Add HPF 18 Hz (Vult Stabile or Bogaudio) after any heavy saturation stage |
| Microtuning not working | Quantizer overriding pitch CV | When using Scala/Surge XT, disable external Quantizer; Surge XT handles tuning internally |
| TZFM pitch drifts upward | Using VCO-1 (not TZ) — clamps at 0 | Switch to Bogaudio FM-OP or NYSTHI TZOP |
| TZFM no effect / dull | Index = 0 or modulator silent | Open FM Depth CV / VCA; check modulator output ±5V |
| TZFM metallic screech | Index > 8, aliasing | Keep Index < 5; add LP 8–12kHz post-FM; enable oversampling |
| Feedback loop howling | Loop gain ≥ 1.0, no limiter | Reduce gain to < 0.94; add Limiter (Vult Comp, threshold ≤6V) in loop |
| Feedback DC runaway | No DC blocker in feedback loop | Add HP 22–30Hz (Bogaudio or Vult) in feedback path |
| NaN / sudden silence in feedback | Filter or reverb became unstable in loop | Kill feedback VCA to 0; restart feedback from gain=0; check for isfinite |
| MPE Pitch Bend wrong scale | Bend range mismatch controller vs module | Match controller + moDllz bend range (24/48/96 semitones) exactly |
| HiRes CC has zipper noise | Standard 7-bit CC used instead of HiRes | Use moDllz HiRcc74 output, not standard CC output |

---

### CPU Optimization — Concrete Numbers

**CPU cost reference (Ryzen 5800X / M1 Pro, VCV 2.5, 48 kHz):**

| Module | 1 Voice | 8 Voice poly | 16 Voice poly |
|---|---|---|---|
| VCO Core Saw | 0.3% | 2.1% | 4.3% |
| Audible Instruments Plaits | 0.9% | 6.8% | 13.5% |
| Vult Freak | 0.6% | 4.5% | 9.1% |
| Valley Plateau (stereo) | 1.8% | — (always mono) | — |
| MindMeld MixMaster | 1.2% base | +0.15%/ch | — |
| moDllz MIDIpolyMPE | 0.4% | 0.7% | 1.1% |

**Rules:**
- Poly 16 ≈ 8–12× CPU vs mono (not 16× due to SIMD, but cache pressure)
- MPE live: 8 voices max. Studio: 12. Bounce-only: 16.
- CPU > 72% real-time → dropout risk, especially during MPE note bursts

**Block size settings:**
- Live / MPE performance: **128–256 samples** (2.7–5.3 ms latency)
- Recording / mixing: **512 samples** (-28% CPU vs 256)
- Render / bounce: **1024–2048** (offline, CPU unconstrained)

**Sample rate:** 48 kHz standard. 96 kHz = ×1.9 CPU — only for heavy FM or oversampled saturation.

**Save CPU:**
```
Reverb/Granular → always on SEND bus, never per-voice (saves factor N)
Scope/Analyzer/Visualizer → disable live (-3–8% CPU)
Vult "High Quality" oversampling → off live, on for render
UI frame rate → 30 Hz instead of 60 Hz on weak GPUs (-5–12%)
Threads → set to physical cores − 2 (e.g., 8-core → 6 threads)
```

**Bounce workflow:**
```
1. Record MPE performance as MIDI (Entrian Timeline)
2. Freeze heavy voices: Voice → AUDIO-16 → WAV 32-bit float
3. Reimport via Simpliciter (1-shot playback) → CPU of that voice = 0%
4. Keep MIDI file for later edits
```

---

