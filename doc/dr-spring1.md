# Spring Doorstop Physical Modeling Investigation

## Executive diagnosis

The uploaded brief frames the core failure mode correctly: the current model can match aspects of the audible “boing” rate while still missing the recognizable object because it does not maintain one physical process that simultaneously explains the slow motion, the repeated lobe articulation, and the long-lived metallic body. The supplied brief reports that the recording sustains a much longer decay than the current model and contains strong energy well above 500 Hz, while the current model remains too concentrated in the low region. fileciteturn0file0

The uploaded engine source confirms why that gap persists. The current engine centers its low structural motion around a 16 Hz base oscillator, then adds a small fixed modal set at 155, 390, 820, and 1650 Hz, plus optional stochastic contact impulses and a short dispersive-delay branch with a nominal 38 Hz round trip. That is enough to make convincing resonant twangs, but it still leaves the low motion, the metallic body, and the lobe articulation too separable. The architecture also bakes in much shorter intrinsic modal decays than the recording suggests, and its “contact” and “waveguide” branches are auxiliary add-ons rather than a single coupled explanation of the object. fileciteturn0file4 fileciteturn0file5

**Directly measured from audio.** In the supplied isolated recording `81458__joedeshon__spring_door_stop_01.wav`, the strongest audible lobe spacing is about **22.0 ms** in the >500 Hz envelope, corresponding to **45.5 Hz** lobe repetition. The same envelope’s autocorrelation shows a second strong peak at **45.0 ms**, implying an underlying slower mechanical periodicity near **22.2 Hz**. Every-other-lobe spectra are slightly more similar than adjacent-lobe spectra, which is what one expects if the audible lobes occur at **both center crossings** of a slower bend rather than once per full bend cycle. The metallic body above 500 Hz remains clearly present between lobes: the median trough level in that band stays about **9×** above the late noise floor, and narrowband troughs remain substantial in the lower metallic peaks. These are new measurements from the supplied recording itself. fileciteturn0file0

**Supported by literature.** Primary spring-vibration literature shows that helical springs are not simple one-mode objects: they support coupled torsional, bending, and extensional motion, and helical or curved waveguides are dispersive. Low-frequency spring behavior can therefore coexist with a richer internal wave or modal body, especially once end conditions, curvature, and coupling are included. Inter-coil contact is a real mechanism in compressed springs, but it ordinarily appears as an additional nonlinear interaction rather than the default explanation for regular, low-rate articulation. citeturn3search2turn3search5turn3search0turn1search2turn10search4turn4search1turn5search4

**Engineering inference.** The most economical explanation is not repeated fresh excitation at 45–47 Hz, and not a purely dispersive travel-time effect by itself. The data fit best when a **slow lateral bend state** modulates the radiation of a **persistent metallic modal body**, with mild crossing-synchronous reinforcement and small asymmetry between opposite crossings. In other words: the spring stores energy in higher resonant structure, while the macroscopic motion periodically **opens a radiation window** twice per cycle.

**Remaining uncertainty.** The recording does not uniquely identify geometry, mounting stiffness, microphone position, or whether the rubber tip or wall contributes to the radiated pattern. Those unknowns matter for absolute radiation strength and the exact distribution of upper modes. They do not, however, overturn the central discrimination above. The brief itself explicitly notes those uncertainties, and the report below keeps them separated from what the audio can support directly. fileciteturn0file0

## Measured findings from the supplied recordings

The empirical analysis below is based primarily on the supplied isolated hit `81458__joedeshon__spring_door_stop_01.wav`, which the brief itself identifies as the clean reference specimen. I also spot-checked the other supplied doorstop recordings as secondary context, but I did not treat them as equally controlled because they contain different microphones, different amounts of room sound, and in some cases multiple strikes or source processing. fileciteturn0file0

![Waveform envelope and spectrogram for the supplied 81458 recording](sandbox:/mnt/data/doorstop_81458_analysis.png)

The recording begins from near-silence and crosses a conservative 1 ms RMS onset threshold at about **86 ms**. The strongest broadband impact packet arrives shortly afterward and lasts on the order of **5 ms** at the top of the >2 kHz attack burst. After that initial packet, the sound is dominated by a recurring articulated decay rather than repeated attack-like events. The average spectrum over the active portion of the sound shows prominent peaks near **377 Hz**, **592 Hz**, **775 Hz**, **1270 Hz**, and **2530 Hz**, closely matching the user’s supplied observations but confirmed here independently from the audio. A secondary ridge cluster also appears around **1.17–1.58 kHz**.

### Empirical answers for the 81458 recording

**Do the 47 Hz lobes correspond to full cycles or center crossings?**  
The direct audio evidence favors **center crossings**, not whole bend cycles. The >500 Hz lobe spacing has a median of **21.98 ms** (**45.49 Hz**), while the same envelope’s autocorrelation has strong peaks at **22.40 ms** and **44.97 ms**. That dual structure is exactly what you expect when the radiated articulation occurs **twice per slower mechanical cycle**. Additional support comes from lobe spectral similarity: adjacent lobe spectra correlate at about **0.726**, while lobe `n` and lobe `n+2` correlate slightly higher, about **0.749**, indicating mild odd/even alternation. That makes sense if alternate lobes correspond to opposite-direction center crossings of the same slower lateral motion.

**Does energy above 500 Hz remain continuous between lobes?**  
Yes. It is strongly modulated, but it is not extinguished between lobes. In the >500 Hz band, the median trough between adjacent lobes remains about **0.21** of the geometric mean of the surrounding lobe peaks, and the median trough is still about **9.2×** the late noise floor. Narrowband continuity is even clearer below 1 kHz: troughs near the ~375 Hz ridge retain a median amplitude around **62%** of the sampled lobe-peak amplitude in that band; the ~586 Hz and ~773 Hz regions retain roughly **27%** and **23%** respectively. Higher ridges around ~1.29 kHz and ~2.53 kHz dip more deeply, but they still do not vanish completely.

**Are later lobes gating persistent modes or re-exciting them?**  
The stronger evidence is for **gating persistent stored energy**, with at most a weak crossing-synchronous reinforcement. The higher-frequency ridges remain visible through troughs, and later lobe spectra are much less broadband than the initial impact. A simple spectral-flux comparison gives a median later-lobe flux only about **28%** of the initial attack flux. Spectral flatness and bandwidth during later lobe peaks and troughs are also much closer to one another than either is to a clean impact transient. If each lobe were a fresh collision or fresh strike, one would expect repeated broadband attack signatures, sharper resets, and stronger discontinuities in the ridges. That is not what the recording shows.

**Do the spectral ridges show dispersion, beating, or coherent pitch drift?**  
They show **mode-dependent evolution**, not one globally coherent chirp. Three measured features matter here. First, the lobe-centered spectral centroid falls over the decay: its median drops from roughly **1.13 kHz** early to roughly **0.92 kHz** late, so the sound gets darker over time. Second, ridge tracking shows different behavior by band: the ~375 Hz ridge stays almost stationary, the ~773 Hz ridge drifts modestly downward, and the ~1.29 kHz region drifts downward more noticeably, while the ~586 Hz and ~2.53 kHz regions are less coherent and sometimes exchange dominance with neighboring peaks. Third, every-other-lobe similarity is slightly stronger than adjacent-lobe similarity, which is more consistent with **alternating radiation geometry and mild beating/coupling** than with a single, clean carrier sliding in pitch. So the best summary is: **weak dispersion and coupling are present; a single coherent pitch drift is not.**

**What are the bandwise decays?**  
Using an upper-envelope fit based on lobe peaks, rather than naive whole-signal Schroeder integration that is biased short by the troughs, I measured approximately these **T20-like upper-skeleton decays**:

- **200–500 Hz:** about **2.77 s**
- **500–1000 Hz:** about **4.62 s**
- **1000–2000 Hz:** about **4.05 s**
- **2000–4000 Hz:** about **2.98 s**
- **4000–8000 Hz:** about **2.86 s**

That pattern matters. The longest persistence is not in the deepest band, but in the **0.5–2 kHz metallic body**, exactly where the current uploaded engine is weakest relative to the recording. The late sound also redistributes spectrally: the lobe-centered energy share in **1000–2000 Hz** drops over time, while **500–1000 Hz** grows relatively more important, which is consistent with a gradual bright-to-mid metallic settling rather than static resonators.

**What changes lobe to lobe?**  
The biggest consistent lobe-to-lobe change is not a complete re-creation of the sound, but a **crossing-dependent reweighting** of persistent content. The odd/even alternation is present but mild. Median adjacent-lobe centroid change is only about **48 Hz**, small relative to the absolute centroid; yet every-other-lobe spectral similarity is slightly higher than adjacent similarity. That is exactly the fingerprint I would expect from a stable body whose radiation weights depend modestly on crossing direction and instantaneous bend phase.

**Is there evidence for intermittent contact?**  
There is **no strong evidence** that intermittent contact is the main cause of the recurring boing lobes in this recording. The periodicity is too regular, the lobe spectra are too continuous, and later lobes lack repeated attack-like broadband resets. That does not mean all contact is absent. A tip-wall or mount nonlinearity may still contribute to the initial attack, specimen-specific roughness, or rare extra ticks. But the recording does **not** require repeated collision to explain the main articulation.

![Autocorrelation and band-decay behavior measured from the supplied 81458 recording](sandbox:/mnt/data/doorstop_81458_autocorr_decay.png)

The other supplied recordings are broadly consistent with this picture, though less cleanly. They show specimen-dependent boing rates in the few-tens-of-hertz range and similarly persistent metallic bodies, but the uncontrolled differences in strike direction, room response, and file processing are large enough that I would not use them to fit hard numerical targets.

## Literature synthesis

The primary literature that mattered most here falls into three clusters: helical-spring wave mechanics, nonlinear/contact spring dynamics, and efficient audio-rate physical modeling.

W. H. Wittrick’s **“On Elastic Wave Propagation in Helical Springs”** (*International Journal of Mechanical Sciences*, 1966, DOI: 10.1016/0020-7403(66)90061-0) is foundational because it shows that helical springs support coupled propagation phenomena that are not captured by a lumped scalar spring constant. Wittrick distinguishes propagation associated with torsion and bending in the wire and shows that extension and rotation are coupled by the helix geometry. That directly supports treating the doorstop as a structure capable of carrying a richer internal body than a single audible low resonance. citeturn3search2

Y. Kagawa’s **“On the Dynamical Properties of Helical Springs of Finite Length with Small Pitch”** (*Journal of Sound and Vibration*, 1968, DOI: 10.1016/0022-460X(68)90190-9) is important because it treats finite-length springs, where boundary conditions matter strongly. That is exactly the regime of a doorstop: a short, mounted helical structure with one constrained end and a free radiating tip does not behave like an infinite or uniform waveguide. citeturn3search5

Lelio Della Pietra’s **“The Dynamic Coupling of Torsional and Flexural Strains in Cylindrical Helical Springs”** (*Meccanica*, 1976, DOI: 10.1007/BF02138004) and the related helical-spring literature it sits inside are directly relevant to the “what couples to what?” question. The decisive point is that bending and torsional behavior are dynamically coupled in cylindrical helical springs, which means a visible sideways wag and a metallic internal body are not separate phenomena by default. citeturn3search0

J. Lee and D. J. Thompson’s **“Dynamic Stiffness Formulation, Free Vibration and Wave Motion of Helical Springs”** (*Journal of Sound and Vibration*, 2001, DOI: 10.1006/jsvi.2000.3169) is especially useful because it explicitly analyzes wave motion and internal resonances in realistic helical springs. Their result that multiple wave types and internal resonances appear even at comparatively low frequencies is a strong argument against an over-simplified low-mode-only body model. citeturn1search2

L. E. Becker, G. G. Chassie, and W. L. Cleghorn’s **“On the Natural Frequencies of Helical Compression Springs”** (*International Journal of Mechanical Sciences*, 2002, DOI: 10.1016/S0020-7403(01)00096-0), together with Krzysztof Michalczyk’s later open-access work on lateral spring vibrations, is the most relevant source family for the slow visible bend state. These papers show that low transverse/lateral spring modes are real, sensitive to axial condition and end treatment, and not trivially identical to the spring’s internal wave resonances. citeturn10search0turn9search0turn9search2

W. G. B. Britton and G. O. Langley’s **“Stress Pulse Dispersion in Curved Mechanical Waveguides”** (*Journal of Sound and Vibration*, 1968, DOI: 10.1016/0022-460X(68)90139-9) and the companion helical-spring dispersion papers are what keep the dispersive-wave explanation on the table at all. They demonstrate experimentally and theoretically that curved and helical mechanical guides disperse broadband pulses. That said, they support dispersion as a contributor to the metallic body, not as a complete explanation of the twice-per-cycle boing articulation in the supplied recording. citeturn5search3turn5search5

Vijay K. Stokes’s **“On the Dynamic Radial Expansion of Helical Springs Due to Longitudinal Impact”** (*Journal of Sound and Vibration*, 1974, DOI: 10.1016/0022-460X(74)90039-X) matters because it shows that impact in helical springs can trigger interaction between different traveling components and can produce radial motion patterns not predicted by a purely static spring view. That strengthens the case for a reduced model in which the low bend state influences the radiation of a distinct persistent body. citeturn5search4

Jamil M. Renno and Brian R. Mace’s **“Vibration Modelling of Helical Springs with Non-Uniform Ends”** (*Journal of Sound and Vibration*, 2012, DOI: 10.1016/j.jsv.2012.01.036) is particularly relevant to a spring doorstop because the doorstop is all about non-uniform ends: a mounted base, a free tip, and a rubber cap. Their work underlines that end geometry and boundary conditions materially reshape the spring’s vibration behavior. citeturn10search4

C. J. Yang, W. H. Zhang, G. X. Ren, and X. Y. Liu’s **“Modeling and Dynamics Analysis of Helical Spring Under Compression Using a Curved Beam Element with Consideration on Contact Between Its Coils”** (*Meccanica*, 2014, DOI: 10.1007/s11012-013-9837-1) is the key contact paper I used. It confirms that inter-coil contact is a plausible nonlinear contribution in helical springs, but it also frames contact as an explicit additional mechanism that should be invoked when the evidence demands it. In the supplied isolated hit, that demand is weak. citeturn4search1turn4search3

For the audio-modeling side, Balázs Bank and László Sujbert’s **“Generation of Longitudinal Vibrations in Piano Strings: From Physics to Sound Synthesis”** (*The Journal of the Acoustical Society of America*, 2005, DOI: 10.1121/1.1868212) is the most directly useful analogy. It shows how one vibratory subsystem can generate or modulate another and how reduced models can preserve a perceptually crucial coupled phenomenon without simulating the whole structure in brute force. citeturn7search4

Finally, V. Välimäki, T. Laakso, and J. Mackenzie’s **“Elimination of Transients in Time-Varying Allpass Fractional Delay Filters with Application to Digital Waveguide Modeling”** (ICMC 1995) and Vesa Välimäki with Matti Karjalainen’s **“Implementation of Fractional Delay Waveguide Models Using Allpass Filters”** (ICASSP 1995) are the practical sources that matter if one does choose a time-varying waveguide path. They explain why time-varying delay structures can click or destabilize if handled carelessly and how to suppress those artifacts. That makes them important as a negative design constraint, even though I do **not** recommend a waveguide-first architecture here. citeturn12search5turn12search3

## Architecture ranking

**Top-ranked: motion-coupled persistent modal body.**  
This is the best fit to the recording and the best match to the literature. It explains a slower lateral bend state, twice-per-cycle articulation at center crossings, continuous high-frequency metallic energy between lobes, mild odd/even lobe differences, and long mid-band persistence. It is also the cheapest architecture that can plausibly hit the evidence. The measured continuity above 500 Hz argues strongly for persistent stored energy, and the spring literature strongly supports coupled low and internal motion. citeturn3search2turn1search2turn10search4

**Second-ranked: time-varying coupled modes.**  
As a reduced implementation class, this is close to the top-ranked model and can work well if formulated carefully. In effect, it says: use a low state to modulate modal frequencies and output weights directly. I rank it below the top choice only because it is easier to overfit and less physically transparent unless the modulation law is tied explicitly to a bend coordinate and a crossing geometry.

**Third-ranked: contact-augmented hybrid.**  
This becomes attractive only after the minimal model above is working. It is plausible for the initial attack and for rougher specimens, and the literature absolutely supports inter-coil contact as a real nonlinear effect. But the isolated 81458 recording does not need repeated periodic contact to explain its main boing gesture. If implemented too early, this branch will likely produce exactly the wrong artifact: crisp repeated ticks where the recording instead has continuous metallic storage. citeturn4search1turn4search3

**Fourth-ranked: dispersive helical waveguide.**  
A physically rich dispersive waveguide is absolutely defensible on paper. The problem is not plausibility; it is discrimination and cost. The supplied recording does show mode-dependent evolution and some dispersive flavor, but it does **not** show a need for a single reflected traveling-wave explanation of the boing lobes. A waveguide-first model is therefore a higher-risk fit under the plugin’s real-time constraints. It remains a valuable ablation or later extension. citeturn5search3turn5search5turn1search2

**Lowest-ranked among viable candidates: repeated center-crossing re-excitation as the main mechanism.**  
A tiny crossing-synchronous reinforcement may help, but the recording does not sound like it is being freshly struck every half-cycle. The persistent continuity of the metallic body and the lack of repeated impact-like broadband resets are the main falsifiers.

## Recommended minimal architecture

The recommended minimal design is a **low-frequency nonlinear lateral bend oscillator driving a persistent metallic resonator bank through crossing-weighted radiation, with only a small initial impact exciter and no periodic contact source by default**.

That is the smallest architecture I judge likely to recreate the supplied evidence without falling into either of the two traps the brief warns about: “just add bass” and “just add tinny high modes.” fileciteturn0file0

### State variables and equations

Let the visible macroscopic bend be represented by displacement \(x(t)\) and velocity \(v(t)\). Let the persistent metallic body be represented by \(K\) second-order resonant coordinates \(q_i(t)\), \(i=1,\dots,K\). Let \(s(t)\) be a short impact envelope and \(z(t)\) a slower pitch-warp or coupling envelope.

Use the continuous-time reduced model

\[
\dot{x}=v
\]

\[
\dot{v}=u_{\mathrm{strike}}(t)-2\zeta_b \omega_b v-\omega_b^2 x-\alpha_3 x^3
\]

\[
\ddot{q}_i + 2\gamma_i \dot{q}_i + \omega_i(z)^2 q_i
= b_i\,u_{\mathrm{imp}}(t)+\varepsilon_i\,\dot{v}
\]

\[
\dot{s}=-\frac{s}{\tau_s}, \qquad \dot{z}=-\frac{z}{\tau_z}
\]

with

\[
\omega_i(z)=\omega_{i0}\,(1-\kappa_i z)
\]

and a crossing-weighted radiation law

\[
g(x,v)=g_0 + g_1\,\phi\!\left(\mathrm{sat}\!\left(1-\frac{|x|}{x_c}\right)\right)
       + g_2\,\mathrm{sat}\!\left(\frac{|v|}{v_c}\right)
\]

where \(\phi(u)=u^2(3-2u)\) for \(u\in[0,1]\).

The audio output is

\[
y(t)=
c_v v + c_a \dot{v}
+\sum_{i=1}^{K}\left(w_{i0}+w_{i1}g(x,v)+w_{i2}\,\mathrm{sat}(v/v_c)\right) q_i
+y_{\mathrm{imp}}(t)
\]

The three important ideas are these:

The **persistent** body is the sum of the \(q_i\), so upper-band energy remains alive between lobes.

The lobe articulation comes mainly from **time-varying radiation weight**, not from fully restarting those modes every time.

A small signed term \(w_{i2}\,\mathrm{sat}(v/v_c)\) creates **odd/even crossing asymmetry**, which matches the measured fact that every-other-lobe spectra are slightly more similar than adjacent lobes.

For discrete time at sample interval \(T=1/f_s\), the bend state can be updated with a damped semi-implicit step:

\[
v_{n+1}=\frac{v_n + T\left(u_n-\omega_b^2 x_n-\alpha_3 x_n^3\right)}
              {1+2\zeta_b \omega_b T}
\]

\[
x_{n+1}=\mathrm{clip}(x_n + T v_{n+1}, -x_{\max}, x_{\max})
\]

Each body mode uses a standard damped resonator recurrence:

\[
q_i[n]=a_{1i}[n]\,q_i[n-1]+a_{2i}[n]\,q_i[n-2]+b_i\,d_i[n]
\]

with

\[
r_i=\exp\!\left(-\frac{6.907755}{T60_i f_s}\right), \quad
\theta_i[n]=\frac{2\pi f_i[n]}{f_s}
\]

\[
a_{1i}[n]=2r_i\cos \theta_i[n], \qquad a_{2i}[n]=-r_i^2
\]

\[
d_i[n]=s[n]\cdot \eta_i + \varepsilon_i\,(v_{n+1}-v_n)
\]

Here \(f_i[n]\) is updated at control rate from the slowly decaying warp state \(z[n]\), then linearly interpolated over the next control block. That keeps the resonator stable and avoids per-sample transcendental calls.

### Initial parameter estimates and defensible ranges

The measured and literature-supported starting point is:

| Parameter | Initial value | Defensible range | Rationale |
|---|---:|---:|---|
| Bend full-cycle frequency \(f_b\) | 23.0 Hz | 21–25 Hz | Twice-per-cycle lobes at about 45–47 Hz from the supplied recording |
| Bend damping ratio \(\zeta_b\) | 0.020 | 0.012–0.040 | Slow visible gesture, not dominant audio decay |
| Cubic stiffness \(\alpha_3\) | small positive | \(0\) to moderate | Allows amplitude-dependent warp without instability |
| Crossing width \(x_c\) | \(0.22x_{\max}\) | \(0.15x_{\max}\)–\(0.35x_{\max}\) | Makes lobes concentrate near center crossings |
| Gate floor \(g_0\) | 0.30 | 0.20–0.45 | Preserves between-lobe continuity |
| Gate gain \(g_1\) | 0.85 | 0.50–1.20 | Sets lobe depth |
| Velocity gate \(g_2\) | 0.25 | 0.10–0.45 | Helps shape crossing lobes |
| Impact decay \(\tau_s\) | 6 ms | 4–15 ms | Isolated attack packet in the recording |
| Warp decay \(\tau_z\) | 180 ms | 100–300 ms | Early spectral settling, not whole-note glide |
| Mode count \(K\) | 6 | 5–7 | More than 7 is not minimal and raises fitting risk |

A practical initial body-mode set for the supplied recording is:

\[
f_i \approx [377,\ 592,\ 775,\ 1270,\ 1580,\ 2530]\ \text{Hz}
\]

These values come directly from the measured peaks and ridge clusters of the isolated hit. The sixth mode around 1.58 kHz is not one of the user’s headline peaks, but it is repeatedly visible in the measured mean spectrum and helps keep the 1–2 kHz body broad rather than “organ-pipe narrow.”

For decay, initialize with longer persistence in the mid bands than in the deepest or brightest bands. A good first pass is:

\[
T60_i \approx [5.5,\ 7.5,\ 7.0,\ 6.5,\ 5.5,\ 4.5]\ \text{s}
\]

Those are intentionally longer than a naive whole-signal Schroeder estimate would suggest, because the troughs in the boing envelope otherwise make the fitted body decay look too short. This longer-midrange ordering is consistent with the measured upper-skeleton band decays and with the brief’s complaint that the current model dies far too quickly. fileciteturn0file0

Mode output weights should start broad, not needle-like. A safe initial pattern is

\[
w_{i0} \in [0.10,0.22],\quad
w_{i1} \in [0.10,0.35],\quad
w_{i2} \in [-0.06,0.06]
\]

with \(w_{i1}\) largest for the 0.5–1.5 kHz region, because that is where the recording’s persistent metallic body is most convincing.

### Energy bounds, stability, sample-rate handling, and retrigger behavior

The internal energy surrogate can be defined as

\[
E_b=\frac12 v^2+\frac12 \omega_b^2 x^2+\frac14 \alpha_3 x^4
\]

\[
E_i=\frac12 \dot{q}_i^2+\frac12 \omega_i^2 q_i^2
\]

\[
E_{\mathrm{tot}}=E_b+\sum_i \lambda_i E_i + \lambda_s s^2
\]

Use a soft energy ceiling, not a hard mute. If \(E_{\mathrm{tot}} > E_{\max}\), scale the **new excitation increments only**, by

\[
\mu=\sqrt{\frac{E_{\max}}{E_{\mathrm{tot}}}}
\]

and leave already-stored energy untouched. That prevents runaway on repeated triggers while avoiding unnatural pumping.

Practical safeguards:

- Clamp \(x\) and \(v\) to physical maxima.
- Enforce \(0 < r_i < 1\) on every resonator.
- Update \(f_i\) and \(a_{1i},a_{2i}\) only at control rate, for example every 16 or 32 samples.
- Interpolate coefficients across the control block.
- If any state becomes non-finite, zero all states immediately and output silence until the next strike.
- Sleep only after all absolute states remain below threshold for at least 50 ms.

Sample-rate handling is straightforward because this architecture is modal, not delay-quantized. Frequencies stay in hertz and decays stay in seconds. On a sample-rate change, recompute \(T\), \(r_i\), and the bend damping constants, but keep the state values. That makes the model much easier to stabilize from **44.1 kHz through 192 kHz** than a time-varying fractional-delay waveguide. If you ever do add a waveguide extension later, the time-varying fractional-delay literature becomes mandatory because coefficient changes in recursive allpass delays can inject transients if their state is not updated consistently. citeturn12search5turn12search3

Retrigger behavior should be **additive**, not reset-based:

- Add strike velocity into \(v\), with sign preserved.
- Add impact energy into \(s\) and \(z\).
- Add strike drive into each mode input \(d_i\).
- Do **not** zero the modes on retrigger.
- Optionally compress the old modal state by a small strike-dependent factor, for example \(q_i \leftarrow 0.95\,q_i\) on very hard retriggers only, to prevent pathological stackup.

That preserves the believable “already vibrating object got hit again” behavior.

### C++-oriented pseudocode and real-time cost

```cpp
struct DoorstopModel {
    static constexpr int K = 6;

    // low bend state
    float x = 0.0f;
    float v = 0.0f;
    float zWarp = 0.0f;      // slow pitch/coupling envelope
    float sImpact = 0.0f;    // short impact envelope

    // resonator states q[n-1], q[n-2]
    float q1[K] = {};
    float q2[K] = {};

    // control-rate coefficients
    float a1[K] = {};
    float a2[K] = {};
    float bDrive[K] = {};

    // target / interpolated coefficients for current control block
    float a1Target[K] = {};
    float a2Target[K] = {};
    float a1Step[K] = {};
    float a2Step[K] = {};

    // fixed parameters
    float modeFreq[K] = {377.f, 592.f, 775.f, 1270.f, 1580.f, 2530.f};
    float modeT60[K]  = {5.5f, 7.5f, 7.0f, 6.5f, 5.5f, 4.5f};

    float fs = 48000.f;
    float T  = 1.0f / 48000.f;

    float bendHz = 23.0f;
    float bendZeta = 0.02f;
    float cubic = 0.02f;     // tune by ear / fit
    float xMax = 1.0f;
    float vMax = 6.0f;

    float g0 = 0.30f;
    float g1 = 0.85f;
    float g2 = 0.25f;
    float xCross = 0.22f;
    float vCross = 1.0f;

    float w0[K] = {0.14f, 0.16f, 0.18f, 0.22f, 0.13f, 0.12f};
    float w1[K] = {0.10f, 0.20f, 0.24f, 0.30f, 0.18f, 0.12f};
    float w2[K] = {0.00f, 0.01f, 0.02f, 0.03f, 0.02f, 0.01f};
    float eps[K]= {0.0005f, 0.0008f, 0.0010f, 0.0012f, 0.0008f, 0.0005f};

    int coeffCounter = 0;
    static constexpr int CONTROL_SAMPLES = 32;

    uint32_t rng = 0x12345678u;
    bool sleeping = true;

    inline float clampf(float x, float lo, float hi) {
        return std::max(lo, std::min(x, hi));
    }

    inline float smoothstep(float u) {
        u = clampf(u, 0.0f, 1.0f);
        return u * u * (3.0f - 2.0f * u);
    }

    inline float softclip5V(float x) {
        // cheap bounded saturator
        float a = 0.25f * x;
        return 5.0f * (a / (1.0f + std::fabs(a)));
    }

    void setSampleRate(float newFs) {
        fs = std::max(newFs, 1000.0f);
        T = 1.0f / fs;
        updateAllModeTargets();
        // copy target directly on SR change
        for (int i = 0; i < K; ++i) {
            a1[i] = a1Target[i];
            a2[i] = a2Target[i];
            a1Step[i] = 0.0f;
            a2Step[i] = 0.0f;
        }
    }

    void strike(float velNorm) {
        velNorm = clampf(velNorm, -1.0f, 1.0f);
        if (velNorm == 0.0f) return;

        sleeping = false;

        float mag = std::fabs(velNorm);
        float shaped = 0.2f * mag + 0.8f * mag * mag;
        float impulse = velNorm * (2.6f + 2.0f * shaped);

        v = clampf(v + impulse, -vMax, vMax);
        sImpact = std::max(sImpact, 0.6f + 0.6f * shaped);
        zWarp   = std::max(zWarp,   0.3f + 0.5f * shaped);

        // broad initial body injection
        for (int i = 0; i < K; ++i) {
            float drive = shaped * (0.10f + 0.04f * i);
            q1[i] += drive * ((velNorm >= 0.0f) ? 1.0f : -1.0f);
        }
    }

    void updateAllModeTargets() {
        for (int i = 0; i < K; ++i) {
            float fi = modeFreq[i] * (1.0f - zWarp * (0.005f + 0.004f * i));
            fi = clampf(fi, 20.0f, 0.45f * fs);

            float r = std::exp(-6.90775527898f / (modeT60[i] * fs));
            float th = 2.0f * float(M_PI) * fi / fs;

            a1Target[i] = 2.0f * r * std::cos(th);
            a2Target[i] = -r * r;
        }

        for (int i = 0; i < K; ++i) {
            a1Step[i] = (a1Target[i] - a1[i]) / float(CONTROL_SAMPLES);
            a2Step[i] = (a2Target[i] - a2[i]) / float(CONTROL_SAMPLES);
        }
        coeffCounter = CONTROL_SAMPLES;
    }

    float process() {
        if (sleeping) return 0.0f;

        if (coeffCounter <= 0) {
            updateAllModeTargets();
        }

        // low bend oscillator
        float wb = 2.0f * float(M_PI) * bendHz;
        float restoring = wb * wb * x + cubic * x * x * x;
        float damping = 2.0f * bendZeta * wb;

        float vNew = (v - T * restoring) / (1.0f + damping * T);
        vNew = clampf(vNew, -vMax, vMax);
        float xNew = clampf(x + T * vNew, -xMax, xMax);
        float acc = (vNew - v) * fs;

        v = vNew;
        x = xNew;

        // decay envelopes
        sImpact *= std::exp(-T / 0.006f);
        zWarp   *= std::exp(-T / 0.18f);

        // crossing-weighted radiation
        float u = 1.0f - std::fabs(x) / std::max(xCross, 1e-6f);
        float g = g0 + g1 * smoothstep(u) + g2 * clampf(std::fabs(v) / std::max(vCross, 1e-6f), 0.0f, 1.0f);
        float vSign = clampf(v / std::max(vCross, 1e-6f), -1.0f, 1.0f);

        float yModes = 0.0f;
        for (int i = 0; i < K; ++i) {
            a1[i] += a1Step[i];
            a2[i] += a2Step[i];

            float drive = sImpact * (0.02f + 0.01f * i) + eps[i] * acc;
            float q0 = a1[i] * q1[i] + a2[i] * q2[i] + drive;

            q2[i] = q1[i];
            q1[i] = q0;

            float rad = w0[i] + w1[i] * g + w2[i] * vSign;
            yModes += rad * q0;
        }
        --coeffCounter;

        // cheap broadband impact
        rng ^= rng << 13;
        rng ^= rng >> 17;
        rng ^= rng << 5;
        float n = (float(int(rng & 0xffff) - 32768) / 32768.0f);
        float yImpact = sImpact * 0.02f * n;

        float yBody = 0.18f * v + 0.0008f * acc;
        float y = yBody + yModes + yImpact;

        float activity = std::fabs(y) + std::fabs(x) + std::fabs(v);
        if (activity < 1e-5f && sImpact < 1e-5f && zWarp < 1e-5f) {
            sleeping = true;
            return 0.0f;
        }

        return softclip5V(y);
    }
};
```

For one voice, this costs roughly:

- low bend state and gate: about **20–25** scalar ops/sample,
- six resonators: about **6 × 9 = 54** core ops/sample,
- impact noise and saturation: about **10–15** ops/sample,
- amortized control-rate coefficient updates: typically **<5** ops/sample.

So the recommended model is on the order of **90–120 scalar floating-point operations per sample**, plus ordinary memory loads/stores. In plugin terms, that is comfortably real-time and likely **cheaper than a stable, time-varying dispersive waveguide with fractional-delay control**.

## System identification and ablation

The fitting workflow should treat the sound as four partially separable components: **impact**, **slow bend schedule**, **persistent metallic body**, and **radiation articulation**.

First, segment strikes using a broadband novelty detector plus a refractory period of at least **120 ms**, so internal boing lobes are not misclassified as new hits. Strike alignment should be done to the first broadband onset, not to the first large lobe peak.

Second, for each isolated strike, extract:

- onset time and short impact duration,
- lobe times from the **>500 Hz envelope**,
- the envelope autocorrelation peaks near one-lobe and two-lobe lags,
- trough-to-peak continuity in >500 Hz and in tracked narrowbands,
- lobe-centered spectra and odd/even similarity,
- ridge tracks near the persistent peaks,
- band-energy trajectories in 200–500, 500–1000, 1000–2000, 2000–4000, and 4000–8000 Hz,
- whole-note overall decay from an **upper skeleton**, not from trough-biased integrated decay only.

Third, fit in stages:

1. **Impact stage**  
   Fit only onset packet duration, attack spectral slope, and initial broadband level.

2. **Bend stage**  
   Fit lobe timing and center-crossing structure. The objective is the lobe schedule and envelope autocorrelation, not static spectrum.

3. **Persistent-body stage**  
   Fit dominant frequencies, band persistence, and trough continuity.

4. **Crossing-radiation stage**  
   Fit lobe depth, odd/even contrast, and lobe-to-lobe spectral reweighting.

A practical multi-objective score is

\[
J = w_L J_{\mathrm{lobe}}
  + w_C J_{\mathrm{continuity}}
  + w_B J_{\mathrm{bands}}
  + w_R J_{\mathrm{ridges}}
  + w_A J_{\mathrm{attack}}
  + w_D J_{\mathrm{decay}}
  + w_O J_{\mathrm{odd/even}}
\]

with:

- \(J_{\mathrm{lobe}}\): robust mean absolute error of lobe times,
- \(J_{\mathrm{continuity}}\): error in trough/peak ratios above 500 Hz,
- \(J_{\mathrm{bands}}\): log-domain band-envelope trajectory error,
- \(J_{\mathrm{ridges}}\): tracked peak frequency and amplitude error,
- \(J_{\mathrm{attack}}\): onset spectral-flux and short-window descriptor error,
- \(J_{\mathrm{decay}}\): upper-skeleton T20/T60 error,
- \(J_{\mathrm{odd/even}}\): difference between adjacent-lobe and skip-two spectral similarity.

Fit **tightly**: lobe timing, >500 Hz continuity, persistent peak locations, gross band persistence.  
Fit **loosely**: exact absolute spectral tilt above 4 kHz, exact per-recording first-impact noise color, and fine peak motion in the highest bands, because those are the parts most contaminated by microphone placement and specimen differences. fileciteturn0file0

### Concrete ablation matrix

Use the supplied harness to compare these variants against the isolated 81458 recording first, then against the wider set.

| Variant | Mechanism set | What it tests | Expected result if hypothesis is right |
|---|---|---|---|
| A | Impact + persistent modes only | Whether static modal decay alone works | Should miss lobe articulation badly |
| B | A + crossing-weighted radiation | Whether gating persistent modes explains boing | Large improvement in lobe structure and continuity |
| C | B + signed odd/even radiation term | Whether opposite crossings differ audibly | Better adjacent-vs-skip-two similarity match |
| D | C + slow shared pitch warp | Whether mild settling improves realism | Better centroid trajectory and early ridge drift |
| E | D + weak acceleration injection into modes | Whether tiny re-excitation is still needed | Small improvement only; large improvement would falsify pure gating |
| F | D + periodic contact impulses | Whether contact explains lobes | If this helps only attack roughness but hurts continuity, contact is secondary |
| G | Dispersive waveguide replacing modal bank | Whether a travel-time model alone can explain body | Likely better chirp, worse peak stability and fitting risk |
| H | Full hybrid: D + optional one-shot contact | Production candidate if needed | Best if attack still lacks specimen roughness after D |

The falsifiable reading of those results is important:

- If **B** already captures the lobe evidence and upper-band continuity, the central hypothesis is supported.
- If only **F** creates convincing lobes, then the current contact skepticism was wrong.
- If only **G** creates the right body evolution, then dispersion is more central than indicated by the present analysis.
- If **E** yields a small but repeatable gain, the right interpretation is “mostly gating, with weak crossing reinforcement,” not full repeated re-excitation.

## Evidence status and remaining uncertainty

**Directly measured from the supplied audio.**  
The main boing lobes in `81458__joedeshon__spring_door_stop_01.wav` repeat at about **45.5 Hz**, while a slower periodicity near **22.2 Hz** is also present in the envelope autocorrelation. The >500 Hz body remains continuous between lobes. Every-other-lobe spectra are slightly more similar than adjacent-lobe spectra. Mid-band persistence exceeds the current uploaded model’s default modal persistence by a large margin. The late sound darkens, but the ridge motion is mode-dependent rather than one coherent global glide. fileciteturn0file0

**Supported by literature.**  
Helical springs support coupled torsional, flexural, and extensional wave behavior; finite length and end conditions materially affect the modal structure; curved and helical guides are dispersive; and coil contact is a plausible but additional nonlinear mechanism. Efficient reduced-order physical models are therefore justified, but the literature does not force a waveguide-only or contact-only interpretation. citeturn3search2turn3search5turn3search0turn1search2turn10search4turn4search1turn5search4turn12search5

**Engineering inference.**  
The repeated boing is best modeled as **center-crossing radiation gating of a persistent metallic body**, generated by a low bend state coupled to several long-lived modes. A tiny amount of crossing-synchronous reinforcement may help. Repeated collision is not needed for the main effect.

**Remaining uncertainty.**  
Audio alone cannot recover exact geometry, wire diameter, pitch, mount compliance, tip-wall boundary condition, or radiation directivity. It also cannot cleanly separate structural radiation from room/microphone coloration. Because of those limits, the proposed parameter values should be treated as **fit seeds**, not specimen-invariant truths. The place where more data would help most is not “more DSP metrics,” but **one controlled dry recording set plus one geometric measurement set**: spring length, coil diameter, wire diameter, mount type, and tip contact condition.