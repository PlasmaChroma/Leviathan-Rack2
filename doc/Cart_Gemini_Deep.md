# **Electromechanical Modeling and Digital Validation of Professional Turntablism Cartridges**

The transition from traditional analog turntablism to high-performance digital vinyl systems (DVS) has fundamentally altered the requirements for signal processing in the DJ booth. While early software emulations focused primarily on the accuracy of timecode tracking, the modern turntablist demands a degree of sonic authenticity that mirrors the physical nuances of legendary hardware. A digital system is no longer judged solely on its latency, but on its ability to replicate the idiosyncratic frequency responses, phase shifts, and non-linear saturation characteristics of specific phono cartridges. This report provides an exhaustive technical framework for the software modeling and validation of four industry-standard cartridges—the Shure M44-7, the Ortofon Concorde MKII Scratch, the Stanton 680 HP, and a specialized Lo-Fi effect profile—alongside the from-scratch development of a digital model for the previously unimplemented Ortofon Q.Bert signature cartridge.

## **Theoretical Foundations of Cartridge Transduction**

To accurately model a phono cartridge in the digital domain, one must first understand the physics of the electromechanical transduction process. A phono cartridge is essentially a velocity-sensitive generator that translates the mechanical vibrations of a stylus tracing a record groove into an electrical signal.1 In moving magnet (MM) designs, which represent the vast majority of turntablist equipment, a permanent magnet is attached to the cantilever.1 As the stylus follows the undulations of the vinyl, the magnet vibrates between two sets of fixed coils, inducing a voltage according to Faraday’s Law of Induction. This law dictates that the induced electromotive force (EMF) is proportional to the rate of change of magnetic flux, which in this context translates to the velocity of the stylus tip rather than its displacement.2

This velocity-dependent nature is the primary reason why phono cartridges require RIAA equalization. Because the cutting head of a record lathe also operates on velocity principles, the amplitude of low-frequency signals on a record would be physically unmanageable without equalization.3 Consequently, records are cut with a significant bass reduction and a treble boost. The reproduction chain—the cartridge and the preamplifier—must provide the inverse curve to restore a flat frequency response.4 However, the cartridge itself is not a perfect transducer; it introduces its own frequency-dependent characteristics governed by its internal electrical impedance and mechanical resonances.

### **The LCR Network Transfer Function**

The electrical behavior of a phono cartridge is defined by its internal inductance (![][image1]) and DC resistance (![][image2]). When connected to a phono preamplifier, these components form a complex LCR network with the preamplifier’s load resistance (![][image3]) and shunt capacitance (![][image4]), as well as the capacitance of the tonearm wiring and interconnect cables (![][image5]).5 This network functions as a second-order low-pass filter with a characteristic resonant peak.3

The resonance frequency (![][image6]) of this electrical system is critical for software validation. It can be calculated as follows:

![][image7]  
For a typical moving magnet cartridge with an inductance of 500 mH and a total load capacitance of 250 pF, the electrical resonance occurs in the high-frequency region, often between 12 kHz and 20 kHz.3 In software, this resonance must be modeled using a biquad filter structure, where the coefficients are derived from the continuous-time transfer function through the bilinear transform.8 If the load capacitance is too high, the resonance moves lower into the audible spectrum, creating an unnatural "brightness" or "etching" in the sound.3 Conversely, if the capacitance is too low, the high-frequency extension may be prematurely rolled off.10

### **Mechanical Resonances and Cantilever Dynamics**

Beyond the electrical network, the mechanical properties of the stylus and cantilever assembly introduce further non-linearities. The cantilever is not infinitely stiff; it possesses its own mass and compliance, which resonate with the record material itself.11 The effective mass of the stylus tip and the cantilever’s elasticity create a mechanical resonance that usually sits between 15 kHz and 25 kHz in high-quality cartridges.1

| Mechanical Element | Electrical Analogue | Impact on Sonic Profile |
| :---- | :---- | :---- |
| Stylus Tip Mass | Inductance (![][image8]) | Determines high-frequency tracking limit |
| Cantilever Stiffness | Capacitance (![][image9]) | Affects resonant frequency location |
| Suspension Damping | Resistance (![][image10]) | Controls the "Q" or sharpness of the resonance |
| Record Compliance | Capacitance (![][image11]) | Varies based on vinyl formulation |

For a turntablist, mechanical stability is often more important than pure high-frequency extension. Cartridges like the Shure M44-7 use a wide-diameter cantilever to ensure that the stylus stays in the groove during aggressive scratching.12 This increased mass, however, lowers the mechanical resonant frequency, contributing to a "warmer" sound with less high-end "shimmer" compared to audiophile cartridges.1

## **Analysis and Modeling of the Shure M44-7**

The Shure M44-7 is the foundational tool of the scratch DJ, renowned for its legendary "skip resistance" and its "warm, fat" sound.12 Although Shure has exited the phono cartridge market, the M44-7 remains the benchmark against which all software emulations are measured.15

### **Electrical Specifications and Profile**

The M44-7 is a high-output cartridge, generating 9.5 mV at a standard reference velocity of 5 cm/s.17 This high output is achieved through a powerful magnet assembly and a high number of coil turns, which in turn results in relatively high inductance and resistance.1

| Parameter | Specification Value | Source |
| :---- | :---- | :---- |
| DC Resistance | 630 ![][image12] | 5 |
| Inductance | 720 mH | 5 |
| Output Voltage | 9.5 mV (RMS @ 1kHz) | 17 |
| Frequency Response | 20 Hz – 17 kHz | 12 |
| Optimum Load | 47 k$\\Omega$ // 400-500 pF | 5 |

Modeling the "warm and fat" characteristic requires a two-stage digital approach. First, the frequency response must reflect the steep roll-off above 17 kHz.17 This is not a simple low-pass filter but a combination of the electrical resonance (peaking around 14-15 kHz with standard loading) and the mechanical damping of the heavy Type S cantilever.3 Second, the "fat" low-end is a psychoacoustic result of the mid-bass emphasis and the slightly higher second-order harmonic distortion produced by the high-output magnet.13

### **DSP Action Items for M44-7 Emulation**

To validate an M44-7 model, the DSP engineer should implement a biquad filter configured as a low-pass shelf with a slight resonant bump at 15 kHz, followed by a soft-clipping saturation stage. The saturation stage should be calibrated to introduce roughly 0.5% to 1.0% Total Harmonic Distortion (THD) at nominal levels, favoring even-order harmonics to emulate the "thick" analog warmth described by users.14 The roll-off should be steep, simulating the physical limits of the 0.7 mil spherical stylus tracing high-frequency undulations.17

## **Ortofon Concorde MKII Scratch: The Energetic Modernist**

The Ortofon Concorde MKII Scratch was designed to fill the void left by the discontinuation of the Shure M44-7, offering even higher output and improved tracking.13 Its sonic profile is described as "energetic," with "bite" and "explosive" sound, particularly suited for bass-heavy modern music.20

### **Electromechanical Characteristics**

The MKII Scratch utilizes a redesigned generator that increases the output voltage to 10 mV.20 This is paired with a highly compliant suspension that allows for a tracking ability of 120 ![][image13]m, which is significantly higher than most traditional cartridges.20

| Parameter | Specification Value | Source |
| :---- | :---- | :---- |
| DC Resistance | 1200 ![][image12] | 20 |
| Inductance | 850 mH | 20 |
| Output Voltage | 10 mV (5 cm/s @ 1kHz) | 20 |
| Frequency Range | 20 Hz – 18 kHz (-3dB) | 22 |
| Tracking Force | 3.0g – 5.0g (4.0g rec) | 20 |

The "energetic" feel of the Ortofon Scratch comes from its superior transient response and a slight emphasis in the 5 kHz to 10 kHz range.21 Unlike the M44-7, which rounds off sharp transients, the Ortofon maintains a "crispness" that helps the DJ hear the "chirp" and "click" of intricate scratch movements.13

### **DSP Action Items for Ortofon Scratch Emulation**

Validation of the Scratch MKII model requires a digital model that emphasizes high-frequency transients. A high-shelf filter with a \+1.5 dB boost starting at 6 kHz, combined with a very high-headroom saturation model, is appropriate. Because the output is 10 mV, the virtual preamplifier model must be capable of handling high-velocity peaks without harsh digital clipping.25 A fast-acting look-ahead limiter or a complex tanh-based saturation function can be used to manage these peaks while maintaining the "punch".27

## **Stanton 680 HP: The Silky Club Standard**

The Stanton 680 HP is a legendary club cartridge, often preferred for its "silky smooth highs" and "mid-bass energy bloom".29 It was historically favored by broadcast and club DJs who required a balance between scratch stability and high-fidelity music playback.31

### **Electrical Architecture and Sonic Bloom**

The 680 HP features a unique four-coil circuit that produces exceptional channel separation of over 30 dB, which is remarkably high for a DJ cartridge.31 This separation contributes to the "silky" sound by providing a wider, more defined stereo image that lacks the "congestion" of lower-end models.29

| Parameter | Specification Value | Source |
| :---- | :---- | :---- |
| DC Resistance | 1300 ![][image12] | 29 |
| Inductance | 930 mH | 29 |
| Output Voltage | 8.0 mV (or 6.0mV in some specs) | 29 |
| Frequency Response | 20 Hz – 20 kHz | 29 |
| Channel Separation | \>30 dB (@ 1kHz) | 31 |

The "mid-bass bloom" of the Stanton 680 HP is a result of a slight resonance in the 200 Hz to 500 Hz region, likely caused by the interaction of the armature mass and the suspension compliance.11 To model this, the DSP should include a low-mid parametric EQ with a wide Q factor, boosting the signal by approximately 1 dB at 350 Hz.

### **DSP Action Items for Stanton 680 HP Emulation**

The "silky" highs are best achieved through a high-order low-pass filter that is critically damped to avoid ringing. The software should also emulate the high channel separation by ensuring a low crosstalk coefficient between the virtual left and right channels.11 A multi-band saturator that adds subtle harmonic content to the low-mids while leaving the high frequencies clean will effectively replicate the "HP" (High Performance) sound.27

## **From-Scratch Development: The Ortofon Q.Bert Model**

The Ortofon Q.Bert signature cartridge is currently unimplemented in many digital platforms, necessitating a ground-up development process based on its unique performance specifications. Co-designed by legendary turntablist DJ Q.Bert, this cartridge is optimized for one thing: battle-style scratching.13

### **Research-Based Specification Profile**

The Q.Bert model is defined by two primary characteristics: an "ultra-high" output voltage and a specialized frequency response that accentuates the mid-range while subduing the high-end.25 The goal of this design is to bring the "vocal" qualities of scratches to the front of the mix while minimizing the hiss and surface noise of worn battle records.25

| Parameter | Derived Specification | Source |
| :---- | :---- | :---- |
| DC Resistance | 1680 ![][image12] | 25 |
| Inductance | 920 mH | 25 |
| Output Voltage | 11 mV (5 cm/s @ 1kHz) | 25 |
| Frequency Response | 20 Hz – 18 kHz (-3dB) | 36 |
| Tracking Ability | 90 ![][image13]m | 25 |
| Stylus Type | Spherical (R 18 ![][image13]m) | 25 |

### **Architectural Design of the Q.Bert DSP Model**

1. **Input Scaling and Saturation**: The 11 mV output is the highest in the group, requiring a robust saturation model.25 In a digital system, this output must be scaled to provide a significant "hot" signal. The DSP should use a soft-knee saturation curve that begins to compress the signal at \-6 dBFS, emulating the magnetic saturation of the cartridge's heavy coils.26  
2. **Mid-Range Accentuation**: The "scratch-forward" sound is achieved through a specific EQ curve. A parametric bell filter centered at 2.5 kHz with a Q of 1.2 and a \+2 dB boost will bring out the "bite" of scratch samples.25  
3. **High-End Suppression**: To minimize surface noise, the model requires a gentle low-pass filter starting at 12 kHz, with a \-3 dB point at 18 kHz.25 This roll-off acts as a natural noise-reduction system for turntablists using older, degraded vinyl.  
4. **Mechanical Damping**: The Q.Bert stylus has a higher compliance than the Scratch MKII (![][image14]), meaning its cantilever is more flexible.25 The software should model this through a slight phase shift in the high-frequency transients, reflecting the physical lag of the flexible armature.11

## **The Lo-Fi Effect Cartridge: Emulating the Aesthetic of Imperfection**

The "Lo-Fi" cartridge model is not based on a high-performance hardware unit but rather on the intentional degradation of audio to mimic vintage, budget, or worn equipment.45 This effect is essential for producers and DJs working within genres like Lo-Fi Hip Hop, where the "character" of the medium is as important as the music.47

### **Components of the Lo-Fi Sonic Engine**

Modeling a Lo-Fi cartridge requires the synthesis of several undesirable analog artifacts into a cohesive creative effect:

1. **Resolution Degradation (Bit-Crushing)**: Many lo-fi aesthetics are tied to the sound of early 12-bit samplers like the SP-1200.47 The DSP must implement bit-depth reduction and a reduction in internal sample rate to 22.05 kHz or lower.45 This introduces aliasing distortion and a "crunchy" texture that defines the genre.48  
2. **Bandwidth Limitation**: A "sadface" EQ curve is used, rolling off the sub-bass below 150 Hz and the highs above 5 kHz.35 This creates a "muffled" or "gramophone" effect that evokes nostalgia.28  
3. **Pitch Instability (Wow and Flutter)**: Analog turntables with poor motor regulation or warped records introduce pitch drift.27 This is modeled via a modulated delay line. "Wow" is represented by a slow, periodic LFO (0.5 Hz \- 2 Hz) modulating the delay time, while "Flutter" uses a faster, more erratic modulation (\>10 Hz).27  
4. **Noise Generation**: A Lo-Fi model is incomplete without surface noise. This should include a combination of synthesized vinyl crackle (impulsive clicks), a constant tape hiss (weighted white noise), and a low-frequency AC hum (60 Hz).28

### **DSP Action Items for Lo-Fi Validation**

The Lo-Fi model should be validated by comparing its output against recorded samples of "low-end" ceramic cartridges or worn-out professional units. The key is "contrast"—the ability to flip from a pristine high-fidelity sound to a gritty, evocative texture.45 The inclusion of a "wear" slider that increases the intensity of the noise and the depth of the Wow/Flutter LFO allows for a dynamic range of lo-fi effects.27

## **Advanced Validation Techniques for Software Modeling**

To ensure that these digital models are accurate representations of the hardware, several validation methodologies must be employed. These go beyond simple frequency response measurements and delve into the temporal and non-linear characteristics of the cartridges.

### **Frequency Response Testing with VSTM**

The VSTM (Variable Speed Turntable Method) is a sophisticated way to validate cartridge response independently of the test record’s characteristics.16 By playing a spot frequency track and sweeping the turntable speed, the developer can record the cartridge's output at various velocities. Because the amplitude of a magnetic cartridge is proportional to velocity, this method allows for the creation of an extremely accurate frequency response curve that can be used to calibrate the software’s biquad filters.16

### **Impulse Response and Convolution**

For cartridges with highly complex mechanical resonances, such as the Stanton 680 HP or the Shure M44-7, capturing an Impulse Response (IR) can provide a level of detail that parametric EQ cannot match.42 Using a specialized test record with a "spark" or "click" pulse, the engineer can capture the entire electromechanical "fingerprint" of the cartridge. This IR can then be used in a convolution engine within the software to provide a perfect linear replica of the cartridge’s sound.27

### **Modeling Non-Linear Saturation and ADAA**

As noted in the Q.Bert and Shure models, high-output cartridges introduce non-linear distortion when the stylus moves at high velocities (e.g., during a fast scratch). In software, this is usually modeled with a hyperbolic tangent (tanh) function.26 However, tanh and other non-linearities introduce aliasing, which can be particularly audible in the high-frequency range.

The use of Anti-Derivative Anti-Aliasing (ADAA) is a modern solution to this problem.42 By using the antiderivative of the saturation function, the DSP can effectively filter out aliasing artifacts in a computationally efficient manner, allowing the "crunch" of the 11 mV Q.Bert model to sound "analog" and smooth even at high scratch speeds.42

## **Comparative Analysis of Model Characteristics**

The following table provides a comprehensive overview of the target sonic profiles and the corresponding DSP strategies for each modeled cartridge.

| Cartridge | Target Sonic Profile | Dominant DSP Strategy | Validation Metric |
| :---- | :---- | :---- | :---- |
| **Shure M44-7** | "Warm / Fat" | Heavy damping \> 17kHz, mid-bass shelf, 2nd-order harmonic saturator. | Flatness up to 15kHz, steep drop thereafter. |
| **Ortofon Scratch** | "Energetic / Bite" | High-frequency transient boost, high-headroom soft-clipping. | Slew-rate response to sharp impulses. |
| **Stanton 680 HP** | "Silky / Bloom" | High channel separation (low crosstalk), mid-bass parametric peak. | Inter-channel phase coherence and crosstalk ratio. |
| **Ortofon Q.Bert** | "Hot / Mid-Forward" | 11mV scaling, parametric boost at 2.5kHz, high-end LPF noise mask. | Prominence of scratch "chirps" in frequency analysis. |
| **Lo-Fi Effect** | "Degraded / Grit" | 12-bit quantization, 22kHz downsampling, modulated Wow/Flutter. | Comparison against vintage sampling hardware. |

## **Practical Implementation and Integration in DVS**

Implementing these models requires a modular DSP architecture. Each cartridge model should be a "plugin" or "profile" within the DVS engine that can be toggled by the user.

### **Gain Staging and Internal Headroom**

Because cartridges like the Q.Bert output significantly more voltage than standard models, the internal gain staging of the software must be handled with care. A "virtual preamplifier" stage should be the first module in the chain, providing a standard \+40 dB of gain (consistent with moving magnet requirements) before passing the signal to the cartridge-specific EQ and saturation modules.3 If the software uses a fixed-point architecture, 32-bit or 64-bit processing is essential to prevent internal clipping when the 11 mV Q.Bert model is pushed to its limits.42

### **Interaction with Timecode Records**

An important consideration for DVS developers is that these cartridge models should ideally be applied to the *audio* signal, not the *timecode* signal. While the tracking engine benefits from the raw, unfiltered input of the cartridge to ensure the highest resolution of the 1 kHz or 2.5 kHz carrier wave, the user-facing audio output is where the cartridge emulation provides its value.13 Applying a heavy Lo-Fi roll-off or a Q.Bert mid-range boost to the timecode signal could potentially degrade tracking performance by filtering out the carrier frequency or introducing phase shifts that the DVS engine interprets as speed changes.

## **Conclusion: Synthesizing Electromechanical Fidelity**

The accurate modeling of phono cartridges for turntablists represents a significant step forward in the quest for digital authenticity. By moving beyond generic filters and embracing the specific electrical and mechanical nuances of hardware like the Shure M44-7, Ortofon Concorde MKII Scratch, and Stanton 680 HP, software developers can provide a tactile and sonic experience that respects the heritage of the medium.

The research and development of the Ortofon Q.Bert model demonstrates how specific performance needs—such as the requirement for mid-range prominence in battle scratching—can be translated into actionable DSP items like parametric EQ and high-output saturation curves. Similarly, the Lo-Fi effect model shows that the "imperfections" of analog playback—Wow, Flutter, and bit-depth reduction—are not just errors to be corrected, but textures to be emulated for creative expression.

Validation remains the cornerstone of this process. Through the use of VSTM measurements, Impulse Response convolution, and ADAA-protected non-linearities, these software models can be verified as true digital twins of their hardware counterparts. As DVS technology continues to evolve, the integration of these high-fidelity electromechanical models will ensure that the art of turntablism remains grounded in the rich, complex, and "warm" world of analog sound.

#### **Works cited**

1. Secrets of the Phono Cartridge, Part Two \- PS Audio, accessed March 20, 2026, [https://www.psaudio.com/blogs/copper/secrets-of-the-phono-cartridge-part-two](https://www.psaudio.com/blogs/copper/secrets-of-the-phono-cartridge-part-two)  
2. A new displacement-sensitive phono cartridge \- Phaedrus Audio, accessed March 20, 2026, [https://www.phaedrus-audio.com/DisC%20article\_for%20Transform%20Media.pdf](https://www.phaedrus-audio.com/DisC%20article_for%20Transform%20Media.pdf)  
3. Hagerman Technology LLC: Cartridge Loading, accessed March 20, 2026, [https://www.hagtech.com/loading.html](https://www.hagtech.com/loading.html)  
4. Impermanence loading a Phono cartridge? \- EEVblog, accessed March 20, 2026, [https://www.eevblog.com/forum/projects/impermanence-loading-a-phono-cartridge/](https://www.eevblog.com/forum/projects/impermanence-loading-a-phono-cartridge/)  
5. 27A1750 (BF) M55E M44G M44E M44-7 M44C \- Shure, accessed March 20, 2026, [https://content-files.shure.com/KnowledgeBaseFiles/27A1750\_BF\_M55E\_M44G\_M44E\_M44-7\_M44C.pdf](https://content-files.shure.com/KnowledgeBaseFiles/27A1750_BF_M55E_M44G_M44E_M44-7_M44C.pdf)  
6. How to set phono stage loading and gain \- AnalogMagik, accessed March 20, 2026, [https://www.analogmagik.com/loading-gain](https://www.analogmagik.com/loading-gain)  
7. Cartridge Loading Calculator | AlignmentProtractor.com, accessed March 20, 2026, [https://alignmentprotractor.com/cartridge-loading-calculator](https://alignmentprotractor.com/cartridge-loading-calculator)  
8. Audio DSP algorithms \- EEVblog, accessed March 20, 2026, [https://www.eevblog.com/forum/fpga/audio-dsp-algorithms/](https://www.eevblog.com/forum/fpga/audio-dsp-algorithms/)  
9. Modeling of Generic Transfer Functions in SystemVerilog. Demystifying the Analytic Model. \- DVCon Proceedings, accessed March 20, 2026, [https://dvcon-proceedings.org/wp-content/uploads/modeling-of-generic-transfer-functions-in-systemverilog-demystifying-the-analytic-model.pdf](https://dvcon-proceedings.org/wp-content/uploads/modeling-of-generic-transfer-functions-in-systemverilog-demystifying-the-analytic-model.pdf)  
10. Gain, Impedance and Loading: What They Mean \- The Groove Man, accessed March 20, 2026, [https://thegrooveman.com/blogs/guides/gain-impedance-and-loading-what-they-mean](https://thegrooveman.com/blogs/guides/gain-impedance-and-loading-what-they-mean)  
11. Equivalent circuit of the dynamic system of a phono cartridge \- Pspatial Audio, accessed March 20, 2026, [https://pspatialaudio.com/analogy.htm](https://pspatialaudio.com/analogy.htm)  
12. Shure M44-7 Cartridge and Stylus | USA \- Music Store, accessed March 20, 2026, [https://www.musicstore.com/en\_US/USD/Shure-M44-7-Cartridge-and-Stylus-/art-DJE0000131-000](https://www.musicstore.com/en_US/USD/Shure-M44-7-Cartridge-and-Stylus-/art-DJE0000131-000)  
13. Scratch Cartridge Comparison \- DJBooth.net, accessed March 20, 2026, [https://djbooth.net/pro-audio/scratch-cartridge-comparison/](https://djbooth.net/pro-audio/scratch-cartridge-comparison/)  
14. Shure M44-7H vs Ortofon Nightclub Concorde \- CDJ & Vinyl DJing \- DJ TechTools Forum, accessed March 20, 2026, [https://forum.djtechtools.com/t/shure-m44-7h-vs-ortofon-nightclub-concorde/50671](https://forum.djtechtools.com/t/shure-m44-7h-vs-ortofon-nightclub-concorde/50671)  
15. Shure M44-7 vs Concorde : r/DJs \- Reddit, accessed March 20, 2026, [https://www.reddit.com/r/DJs/comments/iy9wob/shure\_m447\_vs\_concorde/](https://www.reddit.com/r/DJs/comments/iy9wob/shure_m447_vs_concorde/)  
16. Phono Cartridge Response Measurement Script | Page 11 | Audio ..., accessed March 20, 2026, [https://www.audiosciencereview.com/forum/index.php?threads/phono-cartridge-response-measurement-script.41148/page-11](https://www.audiosciencereview.com/forum/index.php?threads/phono-cartridge-response-measurement-script.41148/page-11)  
17. Shure M44-7 Phono Cartridge \- Warehouse Sound Systems, accessed March 20, 2026, [http://warehousesound.com/shum447.php](http://warehousesound.com/shum447.php)  
18. m44g-user-guide.pdf \- Shure, accessed March 20, 2026, [https://content-files.shure.com/publications/userGuide/en/m44g-user-guide.pdf](https://content-files.shure.com/publications/userGuide/en/m44g-user-guide.pdf)  
19. Nonlinear Distortion is Relatively New to Me. Please Educate Me. : r/audiophile \- Reddit, accessed March 20, 2026, [https://www.reddit.com/r/audiophile/comments/7wq908/nonlinear\_distortion\_is\_relatively\_new\_to\_me/](https://www.reddit.com/r/audiophile/comments/7wq908/nonlinear_distortion_is_relatively_new_to_me/)  
20. Ortofon Concorde Scratch MKII \- Wired Tunes, accessed March 20, 2026, [https://wrdtunes.com/products/ortofon-concorde-scratch-mkii](https://wrdtunes.com/products/ortofon-concorde-scratch-mkii)  
21. ORTOFON CONCORDE MKII SCRATCH \- Single Cartridge with Pre-Installed Stylus, accessed March 20, 2026, [https://www.agiprodj.com/ortofon-concorde-mkii-scratch-single.html](https://www.agiprodj.com/ortofon-concorde-mkii-scratch-single.html)  
22. Ortofon Concorde MKII Scratch \- Turntable Cartridge and Stylus (Single), accessed March 20, 2026, [https://www.canalsoundlight.com/product/ortofon-concorde-mkii-scratch/](https://www.canalsoundlight.com/product/ortofon-concorde-mkii-scratch/)  
23. Ortofon Concorde MKII SCRATCH Twin Pack \- DJ TechTools, accessed March 20, 2026, [https://store.djtechtools.com/products/ortofon-concorde-mkii-scratch-twin-pack](https://store.djtechtools.com/products/ortofon-concorde-mkii-scratch-twin-pack)  
24. Ortofon Concorde MK2 Scratch Single Cartridge | Best Prices \- Samma3a, accessed March 20, 2026, [https://www.samma3a.com/en/ortofon-concorde-mk2-scratch-single-cartridge.html](https://www.samma3a.com/en/ortofon-concorde-mk2-scratch-single-cartridge.html)  
25. Q.Bert – Ortofon, accessed March 20, 2026, [https://ortofon.com/pages/q-bert](https://ortofon.com/pages/q-bert)  
26. Nonlinear Distortion: Explanation and Examples \- Electroagenda \-, accessed March 20, 2026, [https://electroagenda.com/en/transmission-media/nonlinear-distortion-explanation-and-examples/](https://electroagenda.com/en/transmission-media/nonlinear-distortion-explanation-and-examples/)  
27. Lo-Fi FX Plugins: Best Tools for Texture and Character \- Output, accessed March 20, 2026, [https://output.com/blog/lo-fi-fx-plugins](https://output.com/blog/lo-fi-fx-plugins)  
28. Top 5 Production & Mixing Tips for LoFi | Blog \- UJAM, accessed March 20, 2026, [https://www.ujam.com/blog/top-5-production-mixing-tips-for-lofi/](https://www.ujam.com/blog/top-5-production-mixing-tips-for-lofi/)  
29. Stanton 680HP cartridge stylus | LP GEAR, accessed March 20, 2026, [https://www.lpgear.com/product/STNS680HP.html](https://www.lpgear.com/product/STNS680HP.html)  
30. Stanton D6800HP stylus | LP GEAR, accessed March 20, 2026, [https://www.lpgear.com/product/STAD6800HP.html](https://www.lpgear.com/product/STAD6800HP.html)  
31. Stanton \- 680HP Dj Pro \- turntable catridge \- Phono.cz, accessed March 20, 2026, [https://www.phono.cz/en/record-player-turntable-cartridges/stanton-680hp-dj-pro](https://www.phono.cz/en/record-player-turntable-cartridges/stanton-680hp-dj-pro)  
32. STANTON 680 HP DJ PHONO CARTRIDGE AT KABUSA.COM, accessed March 20, 2026, [http://www.kabusa.com/680hp.htm](http://www.kabusa.com/680hp.htm)  
33. Stanton (misc.) \- Loc, accessed March 20, 2026, [https://tile.loc.gov/storage-services/master/mbrs/recording\_preservation/manuals/Stanton%20(Misc.).pdf](https://tile.loc.gov/storage-services/master/mbrs/recording_preservation/manuals/Stanton%20\(Misc.\).pdf)  
34. Stanton 680HP Cartridge & Stylus \- Warehouse Sound Systems, accessed March 20, 2026, [http://warehousesound.com/sta680hp.php](http://warehousesound.com/sta680hp.php)  
35. Lo-Fi sound design techniques \- Top VST plugins and Ableton stock effects for that good old sound \- Wavepusher.com, accessed March 20, 2026, [https://wavepusher.com/lo-fi-sound-design-techniques-top-vst-plugins-and-ableton-stock-effects-for-that-good-old-sound/](https://wavepusher.com/lo-fi-sound-design-techniques-top-vst-plugins-and-ableton-stock-effects-for-that-good-old-sound/)  
36. Ortofon Concorde Q-Bert Twin Pack, DJ Cartridges, 123DJ, accessed March 20, 2026, [https://www.123dj.com/cartridges/qbertwinpack.html](https://www.123dj.com/cartridges/qbertwinpack.html)  
37. Ortofon Q.Bert OM DJ Cartridge with SH-4 Headshell \- DJ TechTools, accessed March 20, 2026, [https://store.djtechtools.com/products/ortofon-om-qbert-premount](https://store.djtechtools.com/products/ortofon-om-qbert-premount)  
38. Ortofon Concorde QBert Single DJ Cartridge \- zZounds.com, accessed March 20, 2026, [https://www.zzounds.com/item--ORTCCQBERTSINGLE](https://www.zzounds.com/item--ORTCCQBERTSINGLE)  
39. Stylus Q.Bert \- Ortofon, accessed March 20, 2026, [https://ortofon.com/products/stylus-q-bert](https://ortofon.com/products/stylus-q-bert)  
40. Ortofon OM QBert Cartridge \- Black/White (Single) \- Store DJ, accessed March 20, 2026, [https://www.storedj.com.au/products/ortofon-om-qbert-cartridge-black-white-single](https://www.storedj.com.au/products/ortofon-om-qbert-cartridge-black-white-single)  
41. Ortofon OM Q.Bert System DJ cartridges \- buy online | USA \- Music Store, accessed March 20, 2026, [https://www.musicstore.com/en\_US/USD/Ortofon-OM-Q-Bert-System/art-DJE0002350-000](https://www.musicstore.com/en_US/USD/Ortofon-OM-Q-Bert-System/art-DJE0002350-000)  
42. Automated Physical Modeling of Nonlinear Audio Circuits For Real-Time Audio Effects—Part I: Theoretical Development | Request PDF \- ResearchGate, accessed March 20, 2026, [https://www.researchgate.net/publication/224600978\_Automated\_Physical\_Modeling\_of\_Nonlinear\_Audio\_Circuits\_For\_Real-Time\_Audio\_Effects-Part\_I\_Theoretical\_Development](https://www.researchgate.net/publication/224600978_Automated_Physical_Modeling_of_Nonlinear_Audio_Circuits_For_Real-Time_Audio_Effects-Part_I_Theoretical_Development)  
43. OM Q.bert \- Ortofon, accessed March 20, 2026, [https://ortofon.com/products/om-qbert](https://ortofon.com/products/om-qbert)  
44. Ortofon OM Q.Bert Turntable Cartridge and Stylus \- Sweetwater, accessed March 20, 2026, [https://www.sweetwater.com/store/detail/OrtQBertOM--ortofon-qbert-turntable-cartridge-and-stylus](https://www.sweetwater.com/store/detail/OrtQBertOM--ortofon-qbert-turntable-cartridge-and-stylus)  
45. \[Music Production\] How to make a lo-fi sounding production in your DAW \- Reddit, accessed March 20, 2026, [https://www.reddit.com/r/WeAreTheMusicMakers/comments/6hikz8/music\_production\_how\_to\_make\_a\_lofi\_sounding/](https://www.reddit.com/r/WeAreTheMusicMakers/comments/6hikz8/music_production_how_to_make_a_lofi_sounding/)  
46. How to Create a Distinctive Lo-Fi Sound \- Sage Audio, accessed March 20, 2026, [https://www.sageaudio.com/articles/how-to-create-a-distinctive-lo-fi-sound](https://www.sageaudio.com/articles/how-to-create-a-distinctive-lo-fi-sound)  
47. Lo-fi: A Hip-Hop Story \- iZotope, accessed March 20, 2026, [https://www.izotope.com/en/learn/lo-fi-a-hip-hop-story](https://www.izotope.com/en/learn/lo-fi-a-hip-hop-story)  
48. 9 ways to add lo-fi processing to your sounds | MusicRadar, accessed March 20, 2026, [https://www.musicradar.com/how-to/lo-fi-processing-tps](https://www.musicradar.com/how-to/lo-fi-processing-tps)  
49. Ortofon MF7 vs Shure M44-7 cartridge comparison video \- with audio \- DJ TechTools Forum, accessed March 20, 2026, [https://forum.djtechtools.com/t/ortofon-mf7-vs-shure-m44-7-cartridge-comparison-video-with-audio/64277](https://forum.djtechtools.com/t/ortofon-mf7-vs-shure-m44-7-cartridge-comparison-video-with-audio/64277)

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAYCAYAAAD6S912AAAA/klEQVR4XmNgGAWDHlgB8W4gfgXE/4H4OxCfAeJ5yIrIAesZIAYao0uQA5gYIC68gS5BLrBggLhuMroEuaCGAWJgELoEueAQEP8CYn50CXIAyJC/DBBDqQJA3gR5twldAgqKgVgcXRAfmMIAMdAZXQIIhID4KLogIXAdiH8CMTeaOCMQLwTibCQxXiDuBeJpQDwHiJWQ5MBAnQHiOlBOQQacQDwJiD8zICIKZMEeII6G8i8BcTiUzWDHADHkAgPEwGdQ/jkg/gAVA+FlMA1A4A7ETxggmYAqABRpi9EFKQFJDKgFhgsQeyLxSQYgr4IiqRGIaxkg4QcK11FABQAARRwuV+vfNusAAAAASUVORK5CYII=>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABYAAAAXCAYAAAAP6L+eAAABK0lEQVR4XmNgGAXDBhgC8W4gfgfE/4H4OpR/FYi/Qfn1QMwK00AqWMYAMVgOSYwTiBug4pOQxEkCj4H4CLogECgzQAx+gi5BDNBngGiuRJcAglwGiNxKdAliQAkDRLMekhgvEEcB8QsGSJgLIskRDXYC8QcGSHg2A/EWBohFIAM9EcpIA9xA/BOIF6OJdwHxMSAWRRMnGvQyQFynjSYOC546NHGiwSkgfgnEjGjiuxggBqejiYNAHhAvBOKpQOyNJgcGsgwQzevRJRgg4QuSS4HylzBAggWUWWZAxSYD8RQoGwxAyesEEP9iQKRRkEGOSGpUgXg7EF8B4s1A7MQASRnfgVgHSR1VgAsDJCNRHYB8cReJrwDEOUh8ikAxAyRsaxkguRJUnowCOgEA9ZE8pZMPkXgAAAAASUVORK5CYII=>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABcAAAAYCAYAAAARfGZ1AAABSElEQVR4Xu2TsSuFURiHX4NBMhgwiIW7UMRkxYI/gVWZmCwolAwUYmAwkpLBQOmy6SbZFBkNJBaZFCWe4z333uPNcM93y6D71FPn/N7zvff7zjlXpMS/pgNP8Bk/8cbPr/HVz2ewPPtAEnZEmzcGWQXO+nwtyKO5w4wNoUm0+b0tFEq7aIMJW4BR0dquLRTKuGiDtiCrwkF8FD2D6qAWRRpfRPd3Dg9Ff8w17c8vi6cS33DL5It4hjUmj2JA9C1HTN7nc7dliVkSbdJq8uw5TJs8igt8wjKTH8vvX+Tu/gIu47zo1tlnv2kQbbBvC6KH6WrDfr4tuv8buOqzXrz04xzuXp/ju+T/IK5Zd7AmhUd4hQfYg/X4gc1+zRiu+3HRuGt5G8z3cAjrgiwxnXjqx7X4gC24kltRJO4wJ3FK9FA3sevHihJ/yhdigkS2urLlGwAAAABJRU5ErkJggg==>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABYAAAAYCAYAAAD+vg1LAAABPElEQVR4Xu2UvyuFURjHn/y8ZDJg0Z2YmYmy2A0Gg27ucu9wxR9AMhlkUywm/gFFZGCiGCmDkpLJZGHk8/ScdO6Te3p7ZdH91Kfe+3xPzz3v+fGKNPl3FHEVz/ERr/AWF0K+iZPhOTMr+I6XOI2dod6K63gT8kKoZ2IPP7Hmg0AbPuOxD1IsijXVJUhxgsu+2Ig+fMMHbHeZZwOHfbERW2KzzTyTrNyLNR71wW/QXdemH2I7n2IWB30xxZNY8y5Xj+nGM2zxQQo9n9p4xgeBXjzEKVev4K7YHu3jWH0s0oOn+IITLhsXO2Ijrj4ndln0bHeITay/bkRAB5TwAq/xQOxMz8vPtyy+4vqnd1GWG11vneFQ+K2XawcHvkfkRDfwVexNdBmOsIzb8aC86BLpLVzDJbHvTDUe0ORv+QK3yjRnl9H95AAAAABJRU5ErkJggg==>

[image5]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAYCAYAAAAPtVbGAAABT0lEQVR4Xu3UPyiFYRTH8ZP/RP4MFkkZTAZ2EQOlGCSjZLAhI8VgMkgpVpHMyiAyMFFkogwiJWVlYOR7es7V69y6Pe69g+H+6tN77jndnvc+7/NekUIKSaQFSzjFIy5wg0mbr6LH6qyyiA+cYwDl1i/GMq5sXmH9P2cLX5j2A0sJnnHoB7GZkbCAblOmHGHON2PSiDfco9TNfFbQ5psxWZPwK7K6w9jcSVik0w/yFT09usCnhBOUKWNo9s3YPElYqNL1k6nCCYr8IDZ6/nWRET+wNOAAfYneMLYxb58HsWP1LXqt/kk1jvGCbjfrknBsOxK9egnv0hQ2rKfX1PHfxajVv6Iv2gTOcIk9CV8al/S3uwZ12MeQ9a7Rb7X+/bRanVNq8Y4yCTuhB0evmtQW5pwmPFjdjler9fnNWp2X6HbqXa9jU8KzWJD07S3kn+UbVRA3oINMH20AAAAASUVORK5CYII=>

[image6]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABEAAAAYCAYAAAAcYhYyAAAA+ElEQVR4XmNgGAV0BxxA3A/EJ4D4AhBro0oTB3qA+BoQtwPxZyB2QpUmDNiB+CUQ90LZ0qjSxAFnIP4PxN7oEsQAdyDeDcR3GSCGgNggTLJXQGAhEN9HFyQVXAbiLeiCpAAeIP7LAIkVsoENAyQ8wtElSAFZDBBDVJHEWIG4iwESyJVAPBeI+5DkMcAsIH6DJgaKaj0gvgrEsUDcBMSLUFSggdNAvA5NTAaIJYD4KwMkO+AFIGf/BuJUdAkGSBjtQRdEBqBENhWIbRkg4QGyFR2A5JvRBZHBAyC+CMQtQDwDVQoOQK6wRxdEBkUMkCx/BogF0eRGGgAAKdAqfv8/uy8AAAAASUVORK5CYII=>

[image7]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAyCAYAAADhjoeLAAAEVUlEQVR4Xu3daaitUxgH8GUeM1wlQxTl0zUlwxfq1E1moUh8kOuDD0KSMsuckMicuMKVSBkimUqSogxxJUmZSolIRInn6X3f7nvX2efec87e55y9u79f/dvvet7dWfecL/dpvXutXQoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMFamIo/URQAAxsPBkdWRNfUNAADGy0t1AQCA8aJhAwAYcxo2AIAxp2EDABhzGjYAgDH3emS7uggAwNJbFvk18l/kl8iR694GAAAAYKN3V+T9yPL6BgAAS2+ryJ3tKwAAY2hF5IS6CADA+Hi8LgAAMD62j/xbFwEAGB95PEUeVdG5KPJq5OHIJr165+7SvH+mvL32rQAAjEI2Zj/3xjtFPu2NJ8lTS5wnJywAwIT4IPJ8b7xb5O/eeFJsGTlxDJKbNyYlAMAEuK80jzGzSetcF7mmNx61VyJfRG6tbwxhm9I0ngtti8hzdXEC7BzZoS4CAJMhD8rN/8wXy7W966cjJ/XGw3gncmFdXAD9lchJ82jk8roIAFB7OXJUe312Gd1xIt/WhXB/aVYPv4+8Vd2bj00jx9bF0nwR/PmRryIfl+YbI06LXNF/04h1831Ymvk2Kxue74jIT3URAKC2a2kan3R15Prevfk6ujSPKgfp734d1hN1IawsTX3rqv5bmfnfNIycL3+ner7VZXbzraoLAAAzyRWhzyK7t+OTy/TjQWbbbL1XF3pm+zNm48tqfHjkn8jmVT0t1KPTnC+PXKldWRdmcF5dAACYSa5A5SPRlCtuO5Zmo8NZkTcj27b3+rJBGuSyutA6JnJbXZyng8r05i/HC9WYDXJKGX6+AyP710UAgFpuchi00vNJZM/ID/WN0jwKfKMuhuMje9TF1h2ladrm6rHILlXtsDK4Ybu4qs3FPWX6iuL6VhcfKsPNl/aOTNVFAIDaA73r23vX2ZCkbFb6n8fK1bd9I7eU5sP8neNK81h1kDympG56boyc2l7nZoc80mSQFXUh7FWm/7wcD2oIc/drJ3dm5u+4oQ0Bs5H/5pnmy7/PAZEX2lr3d8nPCfbl6tqhVQ0AYB15jEh/FalrjnIFrfs8W9YHbUbIR6d/9Mafl+ZbGQbJjQD9BiublBva67969bn4phpnA9mfI1cHT++Nu4Ywd2Z2v9uwcr4L2uucr/+INJvKrgG+qX09t33tnFONAQBm7d7e9ddlbVNS6zdIebTFIPlVW7+X5r15nEd+sX1eL2/vf9e+zlWultWmIh9FXoxcsu6t8m77uqaM7tDaqdKsonXz1Y3gVPuaq49pn/a10/87AwAsiDMjD5ZmY8J8XVqaDQ5zPWg3m64z6uJ6HBK5KvJMGd3mhw25uTTnz71Wpj8O3S/yY1UDABi5PH/sz9Kswi2FPPh3Uq0qHokCAIskH+t1h+8utvzO0mfr4gRYVgYflQIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABstP4HfhXx4RprmEwAAAAASUVORK5CYII=>

[image8]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABoAAAAYCAYAAADkgu3FAAABPklEQVR4XmNgGAWjAAewAuLdQPwKiP8D8XcgPgPE85AVUROsZ4BYZIwuQU3AxADx0Q10CWoDCwaIbyajS1Ab1DBALApCl6A2OATEv4CYH12CmgBk+F8GiGU0BaDgAgVbE7oEFBQDsTi6IDlgCgPEImd0CSAQAuKj6ILkgutA/BOIudHEGYF4IRBnQ/kqQDyDAZKRc4C4hAGS0UEZvgqIu4F4GVQtBlBngPgGpAEZcALxJCD+zIBIICCD5RggJYcZVGwiEG8BYjYgZmGAmAWi4cCOAWL4BajkMyj/HBB/gIqBMLILZYE4GIi3I4kdAWJ/KBtk+QOEFGWgE4groWyQLz4xQOIRBGoZIPEtBeVTBI4BsSWUbQPEh5HkQIkGFF+g0gUUv2QDZgZIeQiLgywgbkVIg+NqJRC7IomNgkEOAMEKOtXBSh6VAAAAAElFTkSuQmCC>

[image9]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABsAAAAYCAYAAAALQIb7AAABZklEQVR4Xu3UPShGURzH8X9e8pKQIiXZzKwSkfK2GWSQvCxKlMXiJZkMshhssphM3onCQlEWg0HpKZlloSx8//2Px+nQk+ta1POrT937/9+nc89zzrki6aTzi1RhBse4wzmuMej6C2h017EyjWecoRU5rp6JOVy6fq6r/zoreMNo2HDJwj12w0bUjIkNpH9fquxjPCxGSRmecIvsoBdmHtVhMUoWxWYV641/mhuxwWrDxl9Hd5sO9CK241KlG5VhMWoSYgPmBXU/+ThERtiIGj0/OlhX2HApwSaavdoItjAgdjaXxDZPj9jOPkJT8mkvBTjAAxqCXr3Ydq/xaoUYxjLWxWZbile0uGdmnW+jB7YfJ7jAmtiZ65OvXwt9uWJcib2Mpl3sdx/ZwJB3Hys6u0f5PJdTYt9Mja69ntsKlLtarHRi27vfQ5u77sAO6tCbfCJGJjHh3SdQ5K513U+xKrY86fzTvAPrjTwrBtdIYwAAAABJRU5ErkJggg==>

[image10]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABwAAAAYCAYAAADpnJ2CAAABbElEQVR4Xu2UvStHURzGH8UgGRQbSjIRmayYvJRJkrIpkyxeF5TBS1n8C6QMMiAhiyQZRQYjAwtKKRaeb8/3urdjPHfS71OfOvf53nvOufece4ACBSJppSf0hX7TO7++pR9+PU9LkgfyYgsasDaTldIFz9czeS480PMwJPXQgI9hIYYWqNPZsEDGoNp2WIhhAuq0OZOV0yH6BK1pRaYWzRF9g9Zrke5DE7CButPb8qGMftKNIF+lF7QqyKPpgd5mNMi7PLfPnStrUMeNQZ6s61yQR3NFn2lRkB/j75sX0xWvDdMpukMHoQnO0DNamTwQUgN1uhsWoA1jtRG/3qQD0E62U2jc8356D/VlnNJOb/9i/90l/UL6U9sAHZl7GughvaF7UCfV0OzfkR51S9CyGPYFXqFnc6MXmkiCfd4+b7fTa+gftt2fC7aG0962t7RD3wYwlqF1tHqdZ9HYIdHm7SZokyRM0gNo0AL/iB8XQ0yAS/jyXgAAAABJRU5ErkJggg==>

[image11]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABUAAAAYCAYAAAAVibZIAAABI0lEQVR4Xu2UvytGURjHv37lJQMGWd5sZnYhKbtBpjfZDJTBiDIZZFUGm3/A4EcGJoqNwaCk5A+wkInP6TnD8dz3vm5eKfV+6lO359t97jmd51ypwb9mANfwDB/wEm9xPuZbOBafC7GKr3iBU9ge6y24gdcxL8X6t+zhBy76INKKT3jogzyWZA3DtmtxjMu+WI0+fMF7bHOZZxMHfbEa27JVFlpBUe5kTYd98FPC6YaGb7ITrsUMln0xj0dZ4w5XT+nEU2z2QR5h/kLTaR9EevEAJ5JamJYjXMFd2VQ0Jbm68ASfcTQNYET2wlBS68EF3JE1nJTdvMwuwmDP4Tle4b5sZivK3p6wiG68wfGvUX3047uyH6yLWdmufpV12c+nwR/wCZnxL8G7CIgNAAAAAElFTkSuQmCC>

[image12]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAwAAAAUCAYAAAC58NwRAAAA6ElEQVR4XmNgGH5AE4i3APEzIL4AxAeA+BgQlwExM0IZBEQB8Xcg3g7ESkjiakB8A4hPAbEcTNANiP8zQExkggkiAWkGiPw5mMABqEAKTAALuMoAUQMG76AcL7g0JljBgKThIpQD8gcusI0BSUM3lNMHl8YEDxiQNMgC8WcgPgsTQAOg4AYpXoksmAMVlEEWhIIqIH4NxBIgDicQqwMxIxAfBuJshDo4OA3EaTAOOxBvhrLNGSCeQwbyDJDwB8UP3HZQ9NcDcS0QP2WA2AYDkUC8Eyq3HCY4nwHifhgWhEkwQNIRstxgAwCn8DQvKiIE5wAAAABJRU5ErkJggg==>

[image13]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAVCAYAAAB/sn/zAAAAwklEQVR4XmNgGAW0AH1APAFNLB2IdyALsAPxByBuQhYEgpNAvBZZwAGI/wOxP5IYJxD/BuIKJDGGGgaIQnEkMVuomCOSGMNWIL6JLMAAMQlkIshkMGAC4vdAPB8mAAWbGSBuhAMtBogV9UhiXAwQz00CYm6YYBoDROFyKJ8RiKcB8TcgrgTiBKg4w0IgfgrEq4B4HwPECU5AnATEp4F4HkzhAyBeBuPgAjIMEGtz0SXQQQQDRKE+ugQ6KAbiM+iC6AAAg7YkenMhtfwAAAAASUVORK5CYII=>

[image14]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFwAAAAYCAYAAAB3JpoiAAAD6ElEQVR4Xu2YWahNYRTHl3key5AhUYbIUIrIcHhAlCEpvLgoSig8oEzXg5JCphBuIteYWSSKkFmGDA/cG/EgmQoh8f/ftb/ut5ezz71ln9vWPb/619n/dc7+9jet9e0jkiNHjv+TSdAwayaRKlB7awbUgrZCb6FX0F6oTegbyeEU1NyaSaIVNBY6Dx01MUchtBFqAY2DPkFFUEP/SwmAfTljzSQxF3oAbYJ+SPoBHwJdEt0BjlnQb2iN5yWBedA0a8ZEf9FFyYXGvl8Ph0s4C/0Ujb+B8sPhMF8k/YAvFJ0MdsbBdMKbsvEkcRmqb82YOQcVi/a/TzhUwnyowJrpiBpw3oA392/SJPCY05NCB+iwNWOmHvQEyhPtf7rxYrYYas10RA14DWiKhPP1YNEGL3qeYy203ngzRbdbNlkKTbBmzPD0swGqKaWrvLP/BXBTNF4mUQOeDp5Y2Nhw4/M08xFaafwb0BHvOiW6Gq9B3UXv465vi67WbtBu0SLI3DmaP8zALai28VISbzusWTxgkAWiY8AJcLSDTnvXGSnvgHeBfomuKEtK9CHGeF4d0UKyKLjmtrwqunOeQy+h2VJalB9Cd6Ftor8l/FwcfE5HX2iX8bLRDifJ7XTWinfQZ6hx4M0QTcHlggN+zJqGutB9iS4KS0QHnMdHx8DA42mHjIIWQ80Cn7vFhwPBjnG3ODZL5oFgGrMvO3G3wz6xKPusEr03Vzs5APUqDWemrAHnytgH7YSqmZiD2+mZ8biyucLdKnJMFH1YHrccbQPPdYCwXa7QqElmnIsg6pniamey6ILyaQl9hV5DDaCn4XBmOODHremxQnRCqnreOu8z/Q/y9wOfFM3hhHnUsR16D1X3PJ6hORA9PI87w6YpH8b9PGqJq50dEp40h6tnewKVGw74CWsGjBetvsyLDlZivhA5uoo2vNzzmIJYRDkg/O0VL/YC2u9dE+bhojQecyXb42lnRDhckgaYtqKIq53HEp40B2sa+01NNbFImMe+QRck/EZJWMX5Kn9HtIoz77EAfZfwjLJgsNHC4Jr32SK65ZhL86R0S/Ke/O704NrB/2lYuHzYUZ4iGomeLvxOM43wTdk+syOudkZC9yS6nUOi7bS2AQsP6HxF5WC7WWIe48CyChMehVzMKj/4DuHDMpcdFD2fF4jen9uXRzauIK54koIeSfhPpqaiL1IDPI8wB3MncXI7mhhX4Wrj+aTk39rhd7jAXH+LoUFBzKef6MRXKMWiRbUiYV7tbc3KgPtvZY4NZBGer1lXKiXu6NXTBrII3wiXWbOywPMsc11FwprRyZo5sgff6nLkyJEI/gBNagRMt2RO4wAAAABJRU5ErkJggg==>