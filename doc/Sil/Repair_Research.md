# Real-Time Cleanup of AI-Generated Music for a VCV Rack Mastering Module

## Recommended v1 scope

The safest and most useful v1 is **not** an offline restoration suite disguised as a mastering module. It should instead target **localized, repeatable, decoder-shaped defects** that survive into the final stereo mix: single-sample or few-sample micro-peaks, fixed-period seam residue, high-band grit or shimmer, narrow-band metallic “ess” energy, unstable high-frequency side information, and final-sample overs. Those are the classes most consistent with what is known about neural codecs, neural vocoders, upsampling artifacts, localized impulsive defects, and true-peak issues. They are also the ones most amenable to very small, transparent, real-time interventions. citeturn19view0turn19view1turn24view0turn31view0turn12view3turn15view1

The strongest v1 recommendation is a **three-core-stage design**: a sample-domain **micro-peak despiker**, a band-limited **high-frequency dynamic smoother**, and an optional **high-band mid/side stabilizer**, followed by a **very light safety peak stage**. I would *not* put a full STFT restoration block, broadband denoiser, declipper, or phase reconstruction engine into v1. Those are either too CPU-heavy, too latency-heavy, or too likely to flatten genuine musical transients and intentional saturation in a mastering context. That recommendation is an engineering inference, but it is grounded in the contrast between localized click restoration methods, onset/transient literature, phase-coherence limitations in vocoders, and the documented aliasing/nonlinearity costs of heavier nonlinear processing. citeturn12view3turn27view0turn23view0turn35search1turn37search17

A good default philosophy is: **only act when multiple defect signatures agree**. In other words, no stage should fire from a single metric alone. A spike must look impulsive *and* out-of-family for the local RMS *and* unlike a broadband musical transient. A high-band smoother should respond to unstable hiss-like or alias-like behaviour, not to cymbal sustain or deliberate exciter tone. A stereo stabilizer should only damp high-band side energy when it becomes unusually decorrelated or unstable from moment to moment. That is the most practical way to make the processor “do no harm.” citeturn12view3turn27view0turn17view0turn17view1

## Artifact taxonomy

**Neural codec residue** usually presents as a faint synthetic haze, buzzy low-level residue, or “processed” texture between transients and around sustained material. In spectrograms, it often appears as smeared fine texture, faint high-band fuzz, or weak repeated structure around harmonics rather than clean, naturally decaying partials. Modern neural codecs explicitly quantize latent frames with residual vector quantization; improved codec papers also discuss codebook collapse, quantizer-dropout side effects, blurry high frequencies, buzzing, and the need for adversarial and multi-resolution spectral losses to recover detail. citeturn19view0turn20view2turn20view0turn20view1

**Vocoder or decoder artifacts** often sound metallic, phasey, buzzy, or strangely “glass-like,” especially on vocals, consonants, and sustained pitched material. On spectrograms they can show unstable harmonic structure, blurred vertical attack definition, or over-smoothed high frequencies. A music-vocoder paper explicitly identifies perceived pitch instability on sustained notes as a consequence of weak horizontal phase coherence in some mel-to-waveform models. Another recent vocoder paper links artifacts to non-ideal upsampling in high-frequency components and to insufficient phase modeling, which it describes as aliasing plus blurring of spectral detail. citeturn23view0turn24view0

**Periodic micro-peaks** are among the best real-time targets. Subjectively they read as tiny ticks, gritty edge noise, or a low-level “zippering” riding on top of the mix. In waveform view they look like isolated needle peaks or very short symmetric or asymmetric spikes. In spectrograms they may show up as weak broadband vertical pinstripes, or as small tone-like peaks if the periodicity couples into the decoder. Upsampling-artifact work shows that transposed convolutions can create periodic patterns at the stride length, while a later Fourier analysis of AI-music artifacts shows systematic small spectral peaks inherent to deconvolution-style modules. citeturn10view0turn31view0

**Frame-boundary seams** sound like tiny clicks, grit, or seam-like discontinuities at a fixed cadence. They tend to be especially audible in decays, held pads, vocals, or sparse sections where a regularly repeating defect is not masked. In the waveform they look like brief slope breaks or kinks; in the spectrogram they can show as weak, repeated broadband vertical marks or cadence-linked comb structure. This is a plausible consequence of framewise or stride-based latent processing because codec papers describe explicit frame rates and architectural latency, and upsampling-artifact work shows periodic structures at stride-related spacings. citeturn19view0turn8view4turn10view0

**Upsampling artifacts and high-frequency shimmer** sound like synthetic fizz, “air” that will not sit still, or bright hash that blooms and disappears unnaturally. Spectrally they show as small repeated peaks, over-bright upper bands, or alternating high-band emphasis and suppression. The upsampling-artifact literature identifies tonal and filtering artifacts from problematic upsampling operators and spectral replicas while upsampling; recent vocoder work specifically says non-ideal upsampling causes aliasing artifacts in high-frequency components. citeturn9view2turn9view4turn24view0

**Transient smearing** is less repairable. It sounds like dulled attacks, mini-reverb before or after transients, soft or “rubbery” drum edges, and consonants that have lost bite. Spectrally, attacks become less vertical and more blurred over time. Phase-vocoder literature has long described transient smearing and reverberation as characteristic artifacts, and high-fidelity codec papers emphasise low-hop losses specifically to improve quick transients, which is indirect evidence that fast attacks are otherwise easy to blur. A mastering-stage module can sometimes *mask* mild smear by removing competing grit around the transient, but it cannot reconstruct genuinely missing attack microstructure. citeturn17view2turn20view1

**Stereo or phase smear** often reads as width that feels unstable rather than wide, side information that gets splashy in the highs, or an image that narrows and widens unpredictably. In mid/side view, the problem usually lives in the upper side band rather than in the full-band mid. Spatial coding literature describes narrowing stereo image artifacts and spatial instabilities when coherence information is not properly preserved, and current stereo-generation evaluation work still uses channel and phase correlation because those measures track perceived wideness and phase behaviour. citeturn17view0turn17view1

**Pre-limited or over-compressed output** is a separate but common AI-music symptom. It sounds loud but airless, with low short-term crest factor and flattened attacks that can still carry inter-sample overs or edge harshness. In waveform view it looks dense and nearly constant; in metering it tends to show low crest factor despite high loudness. The BS.1770 and R128 recommendations exist precisely because peak-sample readings alone miss important overload behaviour, including true peaks between samples and changes introduced by filtering or bitrate reduction. citeturn15view1turn15view2

For **real-time feasibility**, the most realistic targets are isolated micro-peaks, tiny seam clicks, unstable high-band shimmer, narrow metallic sibilant-like bursts, unstable high-band side energy, and final peak overs. The least realistic are wrong notes, missing partial structure, broken vocal formants, severe pitch wandering, globally smeared attacks, or bad arrangement-level decisions. Those defects are not localized enough for a mastering-stage repair block; they are usually symptoms of missing information upstream and are better addressed by regeneration, alternate decoding, or stem replacement. That conclusion is an inference, but it closely follows the fact that classical restoration works best on localized corruptions, while codec and vocoder papers describe losses of phase, detail, and latent information that a downstream stereo processor does not have access to. citeturn12view3turn23view0turn24view0turn19view0

## Likely algorithmic causes

The most plausible common root cause is the **neural codec bottleneck** itself. SoundStream, EnCodec, and improved RVQGAN-style codecs all rely on convolutional encoder-decoder structures plus quantized latent representations, typically residual vector quantization. By design, continuous information is mapped onto discrete codebook entries at a finite frame rate, and the decoder must reconstruct detail that was not transmitted directly. That is fertile ground for low-level residue, especially in the high band and at frame boundaries. Recent AI-music forensics work explicitly frames current generator detectability around shared neural-codec bottlenecks, though that evidence is newer and should be treated as suggestive rather than final. citeturn19view0turn19view4turn20view4turn29view0

A second cause is the **waveform decoder or vocoder architecture**. HiFi-GAN and later models achieve very fast waveform synthesis, but they do so with learned upsampling and adversarial training recipes; more recent BigVGAN and FA-GAN variants exist largely because earlier designs still showed high-frequency artifacts, aliasing, or weak phase detail. BigVGAN adds anti-aliased periodic activations specifically to reduce high-frequency artifacts, and FA-GAN says non-ideal upsampling causes aliasing in high-frequency components while stronger real/imaginary losses help with blurring and phase detail. citeturn22view2turn21view0turn21view1turn24view0

A third cause is **hop-size or frame-boundary discontinuity**. SoundStream explicitly ties architectural latency to its total stride; its default example yields 320-sample frames at 24 kHz, about 13.3 ms. More generally, framewise models and overlap-add decoders can leak regular seam energy when the underlying prediction, interpolation, or overlap behaviour is imperfect. The upsampling-artifact paper is especially helpful here because it shows that periodicity can appear exactly at the stride length of transposed convolutions. If you see repeated spikes at fixed sample intervals, stride or hop cadence should be your first suspicion. citeturn19view0turn10view0

A fourth cause is **upsampling, aliasing, and imaging**. The audio-upsampling paper identifies two major sources: tonal and filtering artifacts from problematic upsampling operators, plus spectral replicas produced during upsampling. It also shows that even in full-overlap settings, non-constant filters can leave a periodic pattern at the stride frequency. The more recent Fourier analysis of AI-music artifacts reinforces that point by proving systematic spectral peaks from deconvolution modules. That combination explains why some AI artifacts present both as tiny time-domain spikes and as faint but fixed spectral lines. citeturn9view2turn10view0turn31view0

A fifth cause is **phase reconstruction weakness**. The music-vocoder paper on stable pitch argues that sustained-note instability comes from lack of horizontal phase coherence, often because a time-domain target with a shift-invariant convolutional model does not force stable phase evolution. Improved codec work also notes that a model trained only with reconstruction losses can sound buzzy because it has not learned to reconstruct phase correctly. This is a strong explanation for metallic vocals, sustained-note wobble, and “not quite attached” consonants. citeturn23view0turn20view0

A sixth cause is **training bias toward loud, already-processed output**. That bias is rarely documented as a single variable, but standards bodies note that filtering and bitrate reduction can raise peak levels, and dynamic-range compression literature emphasizes the large perceptual effect of detector design, knee, timing, and lookahead on transient behaviour. In practice, many generated mixes already arrive density-maximized and close to peak ceiling, so your module should assume it is often cleaning an already-hot master rather than an unprocessed mix bus. citeturn15view1turn15view2turn15view0

## Detection heuristics

For v1, the analysis path should remain **mostly time-domain plus light filterbank analysis**, not full FFT restoration. Classical impulsive-noise work says that when bursts are high-amplitude and localized, simple median or derivative calculations may be sufficient for detection; onset literature likewise treats sharp increases and deviations from local steady state as key cues. That makes a good case for a low-cost detector stack built from first difference, second difference, local RMS, and a few band envelopes. citeturn12view3turn27view0turn38view0

The most important detector is a **micro-peak score**. Per sample, compute:
- first difference `d1 = x[n] - x[n-1]`
- second difference `d2 = x[n] - 2*x[n-1] + x[n-2]`
- very-short local energy `rms_us`
- short local energy `rms_s`
- short-window crest term `|x[n]| / (eps + rms_us)`

Then form a weighted score such as  
`Sspike = a*|d1|/(eps+rms_us) + b*|d2|/(eps+rms_us) + c*max(0, |x|/(eps+rms_us)-Tcrest)`.  
This is not copied from a single paper; it is a practical compression of the derivative-based click logic in restoration work and the sharp-increase logic in onset detection. citeturn12view3turn38view0

You also want a **periodicity boost** for repeated micro-peaks. Keep a small ring buffer of recent spike candidates and accumulate energy by lag. Candidate lags should cover likely decoder cadences, for example a few dozen samples up to a few hundred or low thousands depending on sample rate and suspected stride. If many sub-threshold spikes recur at one lag, lower the firing threshold slightly for that lag family. This is directly motivated by the stride-periodicity findings in transposed-convolution work and by the frame-rate structure in codec papers. citeturn10view0turn19view0

For the high band, use a **cheap filterbank, not a spectrogram**, unless you later profile and find spare CPU. A good compromise is three IIR bands:
- presence band,
- shimmer band,
- air band.

Track each band’s rectified envelope and one smoothed spectral-shape proxy. If you can afford it, compute a tiny “flatness-like” measure from a coarse 4-to-8-band filterbank; if not, use the ratio of max-band envelope to mean-band envelope as a crest surrogate. The justification comes from spectral-flatness literature and from onset work showing that spectral flux is powerful but chosen partly because it is simple and fast. citeturn28search0turn28search1turn27view0

A practical **high-frequency artifact score** is:
`Shf = w1 * highBandEnvelope / (eps + broadbandEnvelope)  
     + w2 * highBandFlux  
     + w3 * flatnessLike  
     + w4 * periodicHFBoost`

where `highBandFlux` can be the positive frame-to-frame change of the high-band envelopes, not a full STFT flux. This is an engineering simplification of spectral-flux and de-essing ideas for a real-time mastering plug-in. citeturn27view0turn34view0

For stereo problems, compute **mid/side high-band envelopes** and a **short-window correlation proxy**. If the side high band suddenly dominates while side correlation becomes unstable, that is a useful signature for synthetic side fizz or phase spray. Stereo-coding literature explicitly links missing coherence to narrowing and instability, and modern stereo-generation papers still use channel and phase correlation as core evaluation features. citeturn17view0turn17view1

False-positive avoidance matters more than raw sensitivity. I would use four vetoes:
- a **transient veto** when broadband onset score is high and energy rise persists longer than a few samples,
- a **tonal veto** when the event sits on stable harmonic energy rather than isolated residue,
- a **saturation veto** when both channels show dense, sustained high-frequency energy that looks like deliberate distortion instead of needle events,
- a **stereo-consistency veto** when the same broadband event occurs coherently in both channels, which is more likely a real transient than side smear.  
This is an engineering inference built from onset-detection behaviour, transient bandwidth observations, and stereo-coherence literature. citeturn12view2turn38view0turn17view0

## Repair algorithms

For isolated events, the best first stage is **local gain smoothing**, not sample replacement. If the defect spans 1 to perhaps 4–8 samples, multiply only a tiny region around the event by a smooth gain notch, for example a raised-cosine attenuation window whose depth depends on confidence. This is gentler than hard interpolation and preserves more of the original transient shape. It also aligns with limiter literature showing that smoother control is a route to fewer frequency artifacts. citeturn37search17turn15view0

For stronger detections, add a **micro-repair mode** that interpolates across only the corrupted center samples while crossfading into the untouched waveform on both sides. Classical restoration literature is built around detecting corrupted sample locations and replacing them with plausible values; for a mastering plug-in, though, you should keep this window tiny and bounded. My recommendation is: only enter interpolation mode when the detector says “localized discontinuity” with high confidence and the repair span remains comfortably below audible musical attack duration. citeturn12view3

For high-band residue, use **dynamic smoothing rather than static EQ**. A de-esser paper describes dynamic processors that damp critical hissing bands relative to average signal level rather than absolute level, which is exactly the right concept here. In a music-cleanup module, the target is not linguistic sibilance alone; it is any short-lived, disproportionately strong, unstable upper-band energy. So the repair should be relative, brief, and program-dependent. citeturn34view0

The repair block I recommend is a **dual-mode high-band smoother**:
- a **wide high-shelf attenuation mode** for broadband shimmer,
- a **narrow moving-notch mode** for metallic, sibilant-like spikes.

The control signal should come from a high-band ratio against broadband level, with fast attack and slower recovery. The notch option is justified by the de-essing paper’s adaptive notch tracking, while the wide mode is safer for non-vocal material. citeturn34view0

For stereo cleanup, use **mid/side high-band stabilization**, not broadband stereo narrowing. Split only the upper band into M/S. When side-high energy becomes unstable, attenuate **S-high** a little, or blend a little of **M-high** into **S-high** using a confidence-weighted coefficient. Keep the action small. The goal is not to reduce width; it is to stop synthetic side fizz from dominating the width cue. Spatial-coherence literature suggests that preserving coherence helps image stability. citeturn17view0turn17view1

For the output stage, use **very light safety limiting or soft clipping**, but be careful: digital soft clipping and limiting can themselves alias if designed carelessly. The signal-processing literature on soft clipping explicitly warns that nonlinear soft-clipping algorithms are major sources of aliasing, and older limiter literature notes that digital clippers and compressors can produce clearly audible aliasing. That means the final stage should be used only as a tiny safety net, not as the main cleanup mechanism. citeturn35search1turn35search3

A small amount of lookahead is technically useful, but only in two places: the **micro-peak repair stage** and the **safety limiter**. Compression literature defines lookahead as applying control computed from the current signal to a delayed version of the signal, and smoother limiter control is known to reduce artifacts. For this module, I would make lookahead **optional** and **tiny**: zero latency by default, with an HQ mode around sub-millisecond to about 1 ms. Any larger and the CPU/latency cost starts to undermine the Rack use case. citeturn37search8turn37search17

## Module architecture and controls

The v1 signal path should look like this:

```text
Input L/R
  -> DC / infrasonic cleanup
  -> analysis tap
  -> micro-peak de-spike / micro-repair
  -> high-band dynamic smoother
  -> optional high-band M/S stabilizer
  -> dry/wet mix
  -> light safety clip / limiter
  -> output trim
  -> Output L/R
```

That architecture follows the evidence well: localized defects first, then spectral polish, then stereo cleanup, then ceiling protection. It also fits the Rack execution model, where `process()` runs every audio frame and sample-rate-dependent coefficients should be refreshed when the module receives a sample-rate-change event. citeturn25view2turn8view6turn25view0

For Rack implementation, keep everything **wait-free inside `process()`** and preallocate all buffers. Use a fixed-size ring buffer for the optional lookahead and periodicity detector. Recompute filter coefficients, detector constants, band boundaries, and buffer lengths inside the sample-rate-change handler, because the API explicitly exposes current `sampleRate` and `sampleTime` in `ProcessArgs` and provides a sample-rate-change callback. citeturn8view6turn25view0turn25view2

I would ship the following controls and defaults:

- **Amount**: master macro over all reduction depths, default **0.35**.  
- **Spike Sensitivity**: threshold trim for `Sspike`, default **0.50**.  
- **Repair Size**: maps to event window, default **0.20 ms max span** at 48 kHz.  
- **High Smooth**: depth of HF dynamic smoothing, default **0.25**.  
- **Stereo Stabilize**: depth of S-high control, default **0.15** and **off by very low amount**.  
- **Dry/Wet**: default **100%**, because the module should be subtle enough to live fully wet.  
- **Delta Listen**: monitor removed signal after the dry/wet split.  
- **Output Trim**: ±6 dB, default **0 dB**.  
These exact defaults are design recommendations, but they are consistent with the localized-restoration and dynamic-filtering evidence gathered above. citeturn12view3turn34view0turn15view0

Metering should be simple and diagnostic rather than decorative:
- **Spike activity LED**: proportional to current despike gain reduction.
- **High smooth LED**: proportional to HF attenuation.
- **Stereo stabilize LED**: proportional to S-high attenuation.
- **Total reduction meter**: a weighted sum of all three.
- **Caution LED**: lights when delta energy exceeds a moving threshold or when transient-veto overrides fire too often, indicating that musical material may be getting hit.  
The caution light is especially important for a “do no harm” processor because the most useful user feedback is not “how much processing is happening,” but “are you starting to clean the music instead of the artifact?” citeturn12view3turn27view0

If lookahead is enabled, the **dry path must be delayed by the same number of samples** before the dry/wet blend. Otherwise delta mode becomes misleading and the mix control will comb-filter. In practice I would expose lookahead only as a hidden quality setting or context-menu option, not as a front-panel control. The front panel should stay musical. citeturn37search8turn25view2

## Core-stage pseudocode

The following pseudocode is the most plausible low-CPU core for v1. It is intentionally simple and avoids allocating or invoking a heavy FFT.

```cpp
struct State {
    float x1L=0, x2L=0, x1R=0, x2R=0;
    float envBroad=0, envHF=0, envSideHF=0;
    float rmsUS=0, rmsShort=0;
    float spikeLagHist[MAX_LAG] = {};
    float lookBufL[LOOK_MAX] = {};
    float lookBufR[LOOK_MAX] = {};
    int lookIdx = 0;
};

inline float onePole(float x, float y, float a) {
    return y + a * (x - y);
}

void processStereo(float inL, float inR, float& outL, float& outR) {
    // 1) cleanup
    float l = dcBlockL.process(inL);
    float r = dcBlockR.process(inR);

    // 2) analysis
    float m = 0.5f * (l + r);
    float s = 0.5f * (l - r);

    float hfL = hpL.process(l);
    float hfR = hpR.process(r);
    float hfM = 0.5f * (hfL + hfR);
    float hfS = 0.5f * (hfL - hfR);

    float d1L = l - st.x1L;
    float d2L = l - 2.f * st.x1L + st.x2L;
    float d1R = r - st.x1R;
    float d2R = r - 2.f * st.x1R + st.x2R;

    st.rmsUS    = onePole(0.5f*(l*l + r*r), st.rmsUS, aUS);
    st.rmsShort = onePole(0.5f*(l*l + r*r), st.rmsShort, aShort);
    st.envBroad = onePole(fabsf(m), st.envBroad, aBroad);
    st.envHF    = onePole(0.5f*(fabsf(hfL)+fabsf(hfR)), st.envHF, aHF);
    st.envSideHF= onePole(fabsf(hfS), st.envSideHF, aSide);

    float spikeScoreL =
        w1 * fabsf(d1L) / (eps + sqrtf(st.rmsUS)) +
        w2 * fabsf(d2L) / (eps + sqrtf(st.rmsUS)) +
        w3 * max(0.f, fabsf(l) / (eps + sqrtf(st.rmsUS)) - crestThr);

    float spikeScoreR =
        w1 * fabsf(d1R) / (eps + sqrtf(st.rmsUS)) +
        w2 * fabsf(d2R) / (eps + sqrtf(st.rmsUS)) +
        w3 * max(0.f, fabsf(r) / (eps + sqrtf(st.rmsUS)) - crestThr);

    float transientVeto =
        transientDetector.update(m, st.envBroad, st.envHF); // broadband onset / flux-like proxy

    float periodicBoost =
        periodicityDetector.update((spikeScoreL > candThr) || (spikeScoreR > candThr));

    float hfScore =
        u1 * st.envHF / (eps + st.envBroad) +
        u2 * hfFlux.update(st.envHF) +
        u3 * hfShape.update(hfL, hfR) +
        u4 * periodicBoost;

    float stereoScore =
        v1 * st.envSideHF / (eps + st.envHF) +
        v2 * sideFlux.update(st.envSideHF) +
        v3 * corrHF.update(hfL, hfR);

    // 3) spike repair
    float gSpikeL = 1.f;
    float gSpikeR = 1.f;
    if (!transientVeto) {
        gSpikeL = spikeGainFromScore(spikeScoreL, periodicBoost);
        gSpikeR = spikeGainFromScore(spikeScoreR, periodicBoost);
    }
    l = microRepairL.apply(l, gSpikeL);
    r = microRepairR.apply(r, gSpikeR);

    // 4) high-band smoother
    float gHF = hfGainFromScore(hfScore, transientVeto);
    hfL *= gHF;
    hfR *= gHF;

    // 5) optional M/S stabilizer on high band only
    float gS = stereoGainFromScore(stereoScore, transientVeto);
    hfS *= gS;
    hfL = hfM + hfS;
    hfR = hfM - hfS;

    // reconstruct after HF path
    l = lowL.process(l) + hfL;
    r = lowR.process(r) + hfR;

    // 6) dry/wet and safety stage
    l = dryWet * l + (1.f - dryWet) * inL_aligned;
    r = dryWet * r + (1.f - dryWet) * inR_aligned;

    l = safetyLimiterL.process(l);
    r = safetyLimiterR.process(r);

    outL = trim * l;
    outR = trim * r;

    st.x2L = st.x1L; st.x1L = l;
    st.x2R = st.x1R; st.x1R = r;
}
```

This structure deliberately uses only sample memory, one-pole envelopes, simple filters, and fixed-size buffers. It is therefore compatible with low-CPU Rack constraints and easy to scale across sample rates. The design is an implementation recommendation, but it is guided by derivative-based click detection, onset-detection smoothing logic, adaptive de-essing, stereo coherence measures, and the Rack processing model. citeturn12view3turn38view0turn34view0turn17view0turn8view6turn25view0

A smaller, stricter pseudocode for the **micro-repair kernel** is:

```cpp
float MicroRepair::apply(float x, float gain) {
    if (gain > 0.999f)
        return x;

    // mild mode: gain notch
    if (!interpMode) {
        return x * gain;
    }

    // strong mode: short interpolation around center sample(s)
    float y0 = history[leftIdx];
    float y1 = future[rightIdx];   // from tiny lookahead, or predicted from local slope if lookahead=0
    float yInterp = hermiteOrLinear(y0, y1, phase01);

    // blend keeps some original attack character
    return lerp(x, yInterp, 1.f - gain);
}
```

The reason to prefer “gain notch first, interpolation second” is that classical restoration treats interpolation as a replacement mechanism once corruption is localized, whereas limiter and dynamics literature encourages smoother control whenever possible to avoid generating new artifacts. citeturn12view3turn37search17

## Validation and known limitations

The test plan should deliberately separate **artifact-removal success** from **musical false positives**. Use clean sine waves, tone sweeps, pink noise, impulses, and click trains to verify detector behaviour; drum loops, plucks, distorted synths, and human-produced mixes to measure false triggers; and AI full mixes plus stem-separated AI material to measure actual cleanup. For periodicity tests, inject synthetic micro-peaks at known lags corresponding to likely stride families and verify that the periodicity detector improves recall without raising drum false positives too far. citeturn10view0turn12view3turn38view0

Objective metrics should include:
- **sample peak** and **true peak** before/after,
- **short-window crest factor**,
- **6–14 kHz RMS** or equivalent upper-band energy,
- **mid/side high-band ratio**,
- **channel correlation / phase correlation**,
- **delta-signal RMS and peak**,
- **false-positive rate** on clean transients,
- **event precision/recall** on injected click trains and seam defects.  
Short-window crest factor is a reasonable dynamics proxy in mastering analysis work, while true-peak behaviour should follow BS.1770-style measurement rather than sample peak alone. citeturn13search8turn15view1turn15view2

Subjective testing matters as much as meters. The most revealing listening modes are:
- original vs processed at matched loudness,
- original vs delta-signal,
- processed with the module bypassed only in transient-rich sections,
- processed in sparse pad or vocal passages where seam cadence is most audible.  
If delta listen contains snare crack, vocal articulation, pick attack, or intended distortion texture, the thresholds are too low. If delta listen mostly contains tiny spits, fizz, and side spray, the module is behaving. That listening protocol is an engineering recommendation, but it is consistent with the localized-restoration premise of the design. citeturn12view3turn17view2

The main **known limitations** are structural. This module will not fix incorrect notes, fractured formants, severe pitch instability, globally smeared attacks, or broken ambience. It can reduce the *audibility* of codec/vocoder residue around those failures, but it cannot reconstruct information that the generator never produced or that the codec never preserved. Likewise, if the incoming file is already aggressively clipped or loudness-maximized, the module can shave residual overs and soften edge harshness, but it cannot restore healthy crest factor without audibly changing punch. citeturn23view0turn20view0turn15view1turn15view2

The highest-confidence v1 feature set is therefore:

- **Time-domain micro-peak detector and despiker** with periodicity awareness. citeturn10view0turn12view3
- **Relative high-band dynamic smoother** tuned for shimmer, grit, and metallic “ess” bursts. citeturn24view0turn34view0
- **Optional high-band mid/side stabilizer** for stereo-phase spray. citeturn17view0turn17view1
- **Delta listen, activity metering, and caution indication** so the user can hear what is being removed. citeturn12view3turn27view0
- **Tiny optional lookahead plus a very light final safety stage**, with matched dry-path delay whenever lookahead is active. citeturn37search8turn37search17turn25view0

If you build that version first, you will get a Rack module that is plausibly excellent at the exact problems you named—**periodic micro-peaks, decoder grit, neural shimmer, and upper-band instability**—while staying honest about what cannot be repaired in real time from a finished stereo render. citeturn31view0turn24view0turn23view0turn15view1