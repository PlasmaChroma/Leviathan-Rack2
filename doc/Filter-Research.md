# **Computational Architectures for Virtual Analog Filter Emulation: A Technical Analysis of Multi-Mode and Dual-Peak Resonant Synthesis in the VCV Rack Ecosystem**

The evolution of digital signal processing within software modular environments has necessitated a transition from idealized, linear filtering algorithms toward sophisticated Virtual Analog (VA) models that replicate the idiosyncratic behaviors of historical hardware. Within the VCV Rack framework, the demand for high-fidelity emulations comparable to industry standards—such as the circuit models found in Ableton’s Auto-Filter—has driven the adoption of Topology-Preserving Transforms (TPT) and Zero-Delay Feedback (ZDF) structures.1 These models, specifically the State Variable Filter (SVF), Sallen-Key (MS2), Diode Filter (DFM), and Transistor Ladder (PRD), offer distinct nonlinear characteristics, resonance profiles, and saturation behaviors that define the sonic character of modern digital synthesis.3  
The contemporary digital filter designer is no longer satisfied with the sterile performance of standard biquad filters. Instead, the focus has shifted toward the "warmth," "growl," and "grit" associated with analog circuitry, which arises from the complex interactions of nonlinear components under high gain and resonance.2 This report investigates the technical implementation of these models, focusing on how they support multi-mode operations, dual-peak resonant architectures, and the specific coding requirements for high-performance execution within the VCV Rack environment.5

## **The Theoretical Foundations of Virtual Analog Modeling**

The fundamental challenge in digitizing analog filter circuits lies in the inherent feedback loops present in their electrical topologies. Analog filters use integrators, primarily capacitors, to achieve filtering by literally integrating the incoming signal over time.5 In the analog domain, these components interact instantaneously within the circuit's feedback loops. Traditional digital filter design, such as the Direct Form I or II biquad structures, introduces a one-sample delay into the feedback path to make the system computable.7 While computationally efficient, this delay causes significant deviations from the analog archetype, particularly resulting in frequency warping as the cutoff approaches the Nyquist limit and instability under fast parameter modulation.5  
To overcome these limitations, modern implementations utilize Zero-Delay Feedback (ZDF) techniques, which aim to solve the feedback equations instantaneously within a single sampling period.7 This methodology is more formally referred to as the Topology-Preserving Transform (TPT), a framework that ensures the digital structure preserves the component interconnections of the original analog circuit.5 By replacing analog integrators with digital equivalents based on trapezoidal integration, the structural integrity of the circuit is maintained, allowing for accurate time-varying behavior even at extreme modulation rates.5

### **Mathematical Framework for Trapezoidal Integration**

The mathematical basis for the TPT is the bilinear transform, specifically applied to the internal states of the filter.9 A continuous-time integrator with the transfer function $H(s) \= \\frac{1}{s}$ is mapped to a discrete-time integrator using the trapezoidal rule:

$$v\[n\] \= v\[n-1\] \+ \\frac{T}{2}(u\[n\] \+ u\[n-1\])$$  
In this equation, $v$ represents the state variable (corresponding to the stored voltage on a capacitor), $u$ is the input signal, and $T$ is the sampling interval.5 When applied to complex circuits like the Moog ladder or the Korg MS-20 VCF, this leads to an implicit system of equations where the output at the current time step depends on its own current value. Solving this system without adding a unit delay requires algebraic manipulation to "look ahead" and determine the output through a closed-form solution or, in nonlinear cases, through iterative numerical methods like Newton-Raphson.11  
The stability of these filters under fast modulation is a primary reason for their adoption in VCV Rack. Unlike Direct Form filters, which can "explode" or produce audible clicks when coefficients are changed abruptly, TPT filters maintain Bounded-Input Bounded-Output (BIBO) stability because their internal states do not get "mangled" under modulation.5 This reliability is indispensable for sound design involving audio-rate frequency modulation (FM) or aggressive envelope following.5

## **Comparative Analysis of Primary Circuit Models**

The Ableton Auto-Filter architecture, which serves as a benchmark for VCV Rack emulations, employs five distinct circuit models inherited from classic hardware designs.3 These models represent a spectrum of "dirtiness" or nonlinear saturation, influencing how the filter handles input gain and resonance peaks.3

### **The State Variable Filter (Clean and OSR)**

The State Variable Filter (SVF) is among the most versatile topologies in digital audio due to its ability to provide simultaneous Low Pass (LP), High Pass (HP), Band Pass (BP), and Notch outputs from a single structure.14 In its "Clean" implementation, the SVF remains mathematically ideal and linear unless a drive stage is explicitly added.16 The "OSR" (Oxford Synthesizer Company) model, based on the OSCar synthesizer, utilizes an SVF topology but incorporates a hard diode clipper in the feedback path to limit resonance and introduce aggressive harmonic distortion.3

| Feature | Clean Model | OSR Model |
| :---- | :---- | :---- |
| **Topology** | Linear State Variable Filter (SVF) | Nonlinear SVF (OSCar-style) |
| **Resonance Behavior** | Transparent and surgical; no peak compression | Squishes low-cut peaks more than high-cut |
| **Distortion Intensity** | 0 (None) | 3 (Moderate-High) |
| **Hardware Reference** | Idealized analog circuit | OSCar (Oxford Synthesizer Co.) |
| **Core Components** | Mathematical Integrators | LM13700 OTA / BJT Buffers |

3  
The SVF is prized for its ultra-smooth modulation properties, making it ideal for filters that require frequent automation of the cutoff frequency without audible artifacts.1 Technically, an "analog modeled" SVF is often a model of the SEM-1A filter or something similar, though the Wasp filter is also an SVF with drastically different nonlinear characteristics.9

### **The Sallen-Key Architecture (MS2)**

The "MS2" model emulates the Sallen-Key topology used in the Korg MS-20 (Revision 2), utilizing an OTA core and parallel soft diode clippers in the feedback loop to tame resonance.2 Unlike the SVF, the MS2 model is characterized by a warm and soft distortion profile.18 When pushed with high input gain (often exceeding \+30 dB), the MS2 model suppresses the resonance peak significantly, causing the frequency contour to flatten out.3  
The Sallen-Key filter consists of two RC sections effectively in series; however, the first stage capacitor is bootstrapped through the output.19 This creates a second-order response where the Quality factor ($Q$) and the gain are closely related.19 In a digital TPT implementation, the MS2 model must account for the specific asymmetric clipping of the MS-20, which provides a rich bass response even when used in high-pass mode.18 The cutoff frequency for a Sallen-Key filter is determined by $f\_c \= \\frac{1}{2\\pi\\sqrt{R\_1 R\_2 C\_1 C\_2}}$, and the $Q$ factor is highly sensitive to component variations, a characteristic that developers must emulate to capture the "unstable" feel of the original hardware.19

### **The Cascade Transistor Ladder (PRD)**

The "PRD" (Prodigy) model replicates the Moog transistor ladder cascade found in synths like the Moog Prodigy.2 This circuit comprises four identical 6 dB/octave sections connected in series with global negative feedback.4 The PRD model is noted for having the "pokiest" resonance among analog models, meaning it retains its sharp resonant peaks even when driven hard.3  
A hallmark of the transistor ladder is the "bass drop" effect: as resonance is increased, the passband gain in the low frequencies decreases.15 This occurs because the feedback signal, which creates the resonant peak at the cutoff frequency, subtracts from the overall signal level.15 Bob Moog exploited the fact that varying the emitter current in a transistor allows the emitter resistance to be varied over a wide range, effectively forming a voltage-controlled RC product.14 Digital implementations of this ladder often use the TPT approach to avoid iterative solving while maintaining the 24 dB/octave slope characteristic.11

### **The Diode Ladder Filter (DFM)**

The "DFM" (Diode Filter Model) emulates circuits such as those found in the Roland TB-303 or the EMS VCS3.26 Diode ladders are structurally similar to transistor ladders but use diodes as variable resistance elements.15 This creates a significantly more complex mathematical model because the stages are inherently coupled; a change in current in one stage affects the dynamic resistance—and thus the cutoff frequency—of every other stage.26  
A unique phenomenon in the diode ladder is filter frequency self-modulation.26 When the input signal is driven hard (above 5mV in analog terms), the voltage swings across the diodes change their dynamic resistance in real-time, effectively modulating the cutoff frequency at the rate of the audio input.26 Modeling this requires solving a system of difference equations derived via the trapezoid integration rule, often necessitating Mathematica or similar tools to handle the "scary looking" equations.26 Developers strongly recommend at least 2x oversampling for diode ladder models to avoid frequency warping and aliasing.26

## **Multi-Mode Mixing and Logic for Dual-Mode Configurations**

A central requirement for an Auto-Filter emulation is the ability to mix multiple filter modes simultaneously.3 In a multi-mode architecture, the goal is often to support combinations of Low Pass, High Pass, Band Pass, and Notch modes, typically two at a time.16

### **SVF Output Mixing and Coefficient Morphing**

The State Variable Filter is the ideal candidate for multi-mode mixing because its internal state variables naturally correspond to different frequency responses.9 In a TPT SVF, the outputs for LP, HP, and BP are calculated during every sample cycle. By applying mixing coefficients ($m\_0, m\_1, m\_2$) to these outputs, a developer can create a continuous "morph" between modes.28

| Target Mode | m0​ (HP) | m1​ (BP) | m2​ (LP) |
| :---- | :---- | :---- | :---- |
| **Low Pass** | 0.0 | 0.0 | 1.0 |
| **High Pass** | 1.0 | 0.0 | 0.0 |
| **Band Pass** | 0.0 | 1.0 | 0.0 |
| **Notch** | 1.0 | 0.0 | 1.0 |
| **Peaking** | 1.0 | 0.0 | \-1.0 |

28  
To support mixing two modes at once—for example, a Notch \+ Low Pass combination—the implementation simply sums the respective weighted outputs.16 This is mathematically represented as $y\_{out} \= \\alpha \\cdot Filter\_A \+ \\beta \\cdot Filter\_B$. In Digital Signal Processing (DSP), parallel filters are typically summed, whereas serial filters are multiplied in the transfer function domain: $H(z) \= H\_1(z) H\_2(z)$.30 Serial arrangements are best when passbands overlap, while parallel arrangements are ideal for passing two distinct bands while blocking the middle.30

### **Pole-Mixing and Higher-Order Mode Generation**

For cascade filters like the Moog Ladder (PRD), multi-mode behavior is achieved through pole-mixing.31 By taking taps from each of the four stages ($lp\_1, lp\_2, lp\_3, lp\_4$) and mixing them with the original input signal, various responses can be synthesized.24 For instance, a 12 dB/octave High Pass response can be generated by mixing the input with specific ratios of the first and second stages: $hp\_2 \= Input \+ 2 \\cdot lp\_1 \+ lp\_2$.31  
This pole-mixing technique allows a single 4-pole circuit to function as a versatile multi-mode filter, though the resonance behavior can become complex at higher feedback levels.31 Developers must ensure that the gains for each stage are correctly balanced so that the overall passband gain remains consistent as the user morphs between modes.31

## **Dual-Peak Architectures and Inverse-Parallel Synthesis**

The "Dual-Peak" or "Twin Peak" architecture represents a specialized form of filtering designed to create complex resonant textures, vocal formants, and "sensory beats".33

### **The Inverse-Parallel Principle**

Inspired by the designs of Rob Hordijk, the Twin Peak architecture uses the inverse-parallel principle.33 In this configuration, the same input signal is passed through two resonant Low Pass filters (A and B), and their outputs are subtracted rather than summed.35 This subtraction results in a Band Pass response characterized by two distinct resonant peaks at the corners of the passband.33  
The logic of this subtraction ($H\_A(z) \- H\_B(z)$) creates a "wave folding" effect on the response curve, allowing the filter to morph from a Low Pass to a variable-width Band Pass.33 A significant advantage of this architecture is that the filters can "pass" each other; regardless of whether Peak A is tuned higher or lower than Peak B, the output remains a valid Band Pass response.33 This allows for "radiate" controls that spread the peaks from a center frequency, creating wide stereo effects and phaser-like tones.36

### **Dual-Peak Modulation and Pinging**

Dual-peak filters are particularly effective for "pinging"—exciting a high-resonance filter with a short trigger or pulse to create a ringing sine wave.33 In a Twin Peak design, each filter can have a separate, tunable ping.33 When the peaks are tuned close to each other, they produce acoustic beats that mimic the dynamics of two-string instruments.33  
Under the hood, these modules often use a 3-pole (18 dB/octave) filter core, as used by Hordijk, to provide a specific balance of resonance and "pluckiness" that differs from the standard 2-pole SVF or 4-pole Ladder.35 The cross-modulation of Filter A's frequency by the output of Filter B further adds to the "outlandish cross-fertilizations" possible with this architecture.33

## **C++ Implementation and SIMD Optimization in VCV Rack**

Developing these sophisticated filters for VCV Rack requires rigorous optimization to support high channel counts and polyphony.12 The VCV Rack SDK provides a Single Instruction, Multiple Data (SIMD) framework that is essential for real-time performance.39

### **Leveraging simd::float\_4 for Polyphonic Processing**

Cables in VCV Rack can carry up to 16 voltage signals.38 To process these efficiently, developers use simd::float\_4, which processes four audio channels simultaneously in a single CPU instruction.39 This yield is critical for multi-mode filters where multiple state updates must occur every sample.6  
Storing data in float\_4 vectors forces the compiler to align elements to 16-byte boundaries, which can make convolution or filter algorithms up to 4 times faster than unaligned scalar code.39 A typical polyphonic filter engine in VCV Rack iterates through the 16 channels in batches of four:

C++

for (int c \= 0; c \< channels; c \+= 4\) {  
    simd::float\_4 input \= simd::float\_4::load(\&inputs.getVoltages()\[c\]);  
    // Filter processing logic here...  
    output.store(\&outputs.getVoltages()\[c\]);  
}

6

### **Branchless Logic with simd::ifelse**

In a SIMD context, standard if statements are problematic because they cause branching, which the CPU cannot easily vectorize.39 To handle conditional logic—such as clipping or selecting between filter modes—developers use simd::ifelse(mask, value\_if\_true, value\_if\_false).40  
This is particularly relevant for emulating nonlinearities like the clippers in the MS2 or OSR models. For example, applying a hard clip to a signal vector without branching:

C++

simd::float\_4 clipped \= simd::clamp(input, \-10.0f, 10.0f);  
input \= simd::ifelse(out\_of\_bounds\_mask, clipped, input);

40  
Nonlinear processes like wave-folding or distortion often require Anti-derivative Anti-Aliasing (ADAA) to reduce artifacts.11 ADAA works by calculating the antiderivative of the nonlinearity and using the average change over a sample period to smooth out sharp discontinuities that would otherwise cause aliasing.11

### **SIMD-Compatible Math Approximations**

Transcendental functions like sin(), tan(), and pow() are computationally expensive in tight DSP loops.43 VCV Rack provides fast SIMD-optimized approximations, such as dsp::exp2\_taylor5(x), which calculates $2^x$ with high precision but lower CPU cost than standard library functions.11 This is vital for the 1V/octave tuning conversion required for filter cutoffs.44

| VCV DSP Utility | Description | Use Case |
| :---- | :---- | :---- |
| exp2\_taylor5 | Fast 5th-order Taylor approximation of $2^x$ | 1V/oct frequency conversion |
| tan\_1\_2 | High-precision approximation of $\\tan(x)$ | Pre-warping filter coefficients |
| rcp\_newton1 | Fast reciprocal ($1/x$) with Newton refinement | Solving TPT feedback equations |
| rsqrt\_newton1 | Fast reciprocal square root | Normalizing filter gains |

11

## **Tuning, Calibration, and Voltage Standards**

To ensure that a filter module behaves predictably within the VCV Rack ecosystem, it must adhere to established voltage and tuning standards.38

### **The 1V/Octave Standard for Frequency**

VCV Rack follows the 1V/oct (volt per octave) standard for CV control of frequency.38 The relationship between frequency ($f$) and voltage ($V$) is defined by the exponential formula $f \= f\_0 \\cdot 2^V$.38  
For audio-rate filters, the baseline frequency ($f\_0$) is Middle C (C4), defined as MIDI note 60 or 261.6256 Hz.38 At its default position, the filter cutoff knob should correspond to 0V.47 This allows users to play the filter "chromatically" when it is in self-oscillation.12 Low-frequency oscillators (LFOs) and clocks, conversely, use a baseline of 120 BPM ($f\_0 \= 2$ Hz).38

### **Normalized Frequency and Pre-Warping**

In a digital TPT filter, the cutoff frequency must be pre-warped to compensate for the compression of the frequency axis inherent in the bilinear transform.5 The normalized frequency coefficient ($g$) is calculated as:

$$g \= \\tan\\left(\\frac{\\pi f}{f\_s}\\right)$$  
where $f$ is the target cutoff frequency in Hz and $f\_s$ is the sampling rate.25 For filters that support self-oscillation, the damping coefficient ($k$ or $Q$) must be carefully calibrated to ensure that the filter enters oscillation at a consistent point, typically represented as a voltage between 0V and 10V.12 Some modules add a small amount of internal noise (e.g., \-120 dB) to the input to "bootstrap" the oscillation.11

### **Voltage Levels and Saturation**

Rack attempts to model Eurorack standards where 10V peak-to-peak is the norm for audio signals.38

* **Audio Outputs:** Should typically be $\\pm 5$V.38  
* **CV Modulation:** Can be unipolar (0V to 10V) or bipolar ($\\pm 5$V).38  
* **Output Saturation:** While VCV Rack allows voltages to exceed $\\pm 12$V (the physical limit of Eurorack power rails), it is recommended to implement analog-style output saturation for a more authentic sound.38

Modules should also check for NaNs (Not a Number) or infinite values in their DSP loops, especially in unstable high-resonance states, and return 0V to prevent system crashes.12

## **Dynamic Modulation: LFOs, Envelopes, and Resonators**

The Auto-Filter character is largely defined by its movement. In the latest versions of these filters (e.g., Live 12.2), additional features like Dry/Wet controls and enhanced Drive parameters have been introduced to provide more precise blending of processed and unprocessed signals.16

### **Envelope Following and Phase Issues**

An envelope follower translates the amplitude of an incoming audio signal into a control voltage.27 This CV then modulates the filter cutoff, creating the classic "wah-wah" or "auto-wah" effect.21 However, designers must be wary of phase shifts; steeper filter cuts affect the phase of a signal more significantly.3 This can lead to nulled frequencies or negative correlation when a filter is used in parallel processing.3 Linear phase filters avoid this but at the cost of higher latency and a lack of "analog" character.51

### **Vowel and Formant Filtering**

Modern emulations often include a vowel or formant filter mode.16 This is typically implemented as a bank of resonant peaks (formants) that simulate the human vocal tract.34 By using an LFO to sweep through these formants, designers can create talking basslines or rhythmic vocal wobbles.52 This multi-peak behavior is functionally a parallel sum of multiple band-pass filters, each tuned to a specific frequency peak corresponding to a vowel like "A," "E," or "O".34

| Filter Type | Behavior | Sound Design Use |
| :---- | :---- | :---- |
| **Comb Filter** | Creates a series of harmonically related notches | Flanging, metallic textures |
| **Vowel Filter** | Parallel peaks simulating formants | Talking synths and basslines |
| **Resampling Filter** | Adds artifacts from downsampling | Lo-fi and digital grit |
| **Notch \+ LP** | Combines frequency rejection with treble cut | Dark, "hollow" synth pads |

16

## **Synthesis of Virtual Analog Research and Design Implications**

The development of a VCV Rack filter module comparable to Ableton’s Auto-Filter requires a multifaceted approach that combines circuit modeling, mathematical integration, and SIMD optimization.2 The shift toward Zero-Delay Feedback (ZDF) and Topology-Preserving Transforms (TPT) has redefined the quality of digital filters, allowing them to remain stable and "musical" even under extreme conditions.5  
By understanding the distinct personalities of the SVF, MS2, DFM, and PRD models, a developer can offer a range of characters—from the surgical precision of the Clean model to the "screaming" resonance of the Sallen-Key and the "squelch" of the diode ladder.3 The implementation of multi-mode mixing through coefficient morphing and dual-peak architectures through inverse-parallel routing opens up vast sound design possibilities, enabling the creation of complex textures that mimic physical resonators.28  
Ultimately, the success of these emulations rests on the developer's ability to balance CPU efficiency with numerical accuracy.12 The use of SIMD processing, ADAA for nonlinearities, and fast math approximations ensures that these modules can be used in polyphonic patches without overwhelming the user's system.6 As software modular synthesis continues to mature, these advanced computational architectures will remain the cornerstone of high-fidelity virtual analog design.7

#### **Works cited**

1. Cytomic on Ableton Partnership, accessed April 16, 2026, [https://www.ableton.com/en/blog/cytomic-on-ableton-partnership/](https://www.ableton.com/en/blog/cytomic-on-ableton-partnership/)  
2. News \- Cytomic, accessed April 16, 2026, [https://cytomic.com/category/news/](https://cytomic.com/category/news/)  
3. Ableton's Auto Filter— Comparing Circuit Models | PerforModule, accessed April 16, 2026, [https://performodule.com/2019/03/07/abletons-auto-filter-comparing-circuit-models/](https://performodule.com/2019/03/07/abletons-auto-filter-comparing-circuit-models/)  
4. Non-Linear Digital Implementation of the Moog Ladder Filter, accessed April 16, 2026, [https://dafx.de/paper-archive/2004/P\_061.PDF](https://dafx.de/paper-archive/2004/P_061.PDF)  
5. Fast Modulation of Filter Parameters | SoloStuff, accessed April 16, 2026, [http://www.solostuff.net/wp-content/uploads/2019/05/Fast-Modulation-of-Filter-Parameters-v1.1.1.pdf](http://www.solostuff.net/wp-content/uploads/2019/05/Fast-Modulation-of-Filter-Parameters-v1.1.1.pdf)  
6. Would Highway be any good for implementing SIMD stuff in a VCV plugin? \- Development, accessed April 16, 2026, [https://community.vcvrack.com/t/would-highway-be-any-good-for-implementing-simd-stuff-in-a-vcv-plugin/19135](https://community.vcvrack.com/t/would-highway-be-any-good-for-implementing-simd-stuff-in-a-vcv-plugin/19135)  
7. Building a ZDF 2pole state variable filter : r/DSP \- Reddit, accessed April 16, 2026, [https://www.reddit.com/r/DSP/comments/1685cll/building\_a\_zdf\_2pole\_state\_variable\_filter/](https://www.reddit.com/r/DSP/comments/1685cll/building_a_zdf_2pole_state_variable_filter/)  
8. When to choose a ZDF \- DSP and Plugin Development Forum \- KVR Audio, accessed April 16, 2026, [https://www.kvraudio.com/forum/viewtopic.php?t=495256](https://www.kvraudio.com/forum/viewtopic.php?t=495256)  
9. Should I be interested in something else than TPT/ZDF filters at this point of time? \- DSP and Plugin Development Forum \- KVR Audio, accessed April 16, 2026, [https://www.kvraudio.com/forum/viewtopic.php?t=601657](https://www.kvraudio.com/forum/viewtopic.php?t=601657)  
10. How can I take this ZDF state variable filter and get a peaking filter out of it? : r/DSP \- Reddit, accessed April 16, 2026, [https://www.reddit.com/r/DSP/comments/9xyz6o/how\_can\_i\_take\_this\_zdf\_state\_variable\_filter\_and/](https://www.reddit.com/r/DSP/comments/9xyz6o/how_can_i_take_this_zdf_state_variable_filter_and/)  
11. Fundamental/src/VCF.cpp at v2 · VCVRack/Fundamental · GitHub, accessed April 16, 2026, [https://github.com/VCVRack/Fundamental/blob/v2/src/VCF.cpp](https://github.com/VCVRack/Fundamental/blob/v2/src/VCF.cpp)  
12. Self-oscillating SVF Questions \- Development \- VCV Community, accessed April 16, 2026, [https://community.vcvrack.com/t/self-oscillating-svf-questions/17896](https://community.vcvrack.com/t/self-oscillating-svf-questions/17896)  
13. Time Varying BIBO Stability Analysis of Trapezoidal Integrated Optimised SVF v2, accessed April 16, 2026, [http://www.rossbencina.com/code/time-varying-bibo-stability-analysis-of-trapezoidal-integrated-optimised-svf-v2](http://www.rossbencina.com/code/time-varying-bibo-stability-analysis-of-trapezoidal-integrated-optimised-svf-v2)  
14. state variable VS ladder filter \- Moog Music General Topics Forum, accessed April 16, 2026, [https://forum.moogmusic.com/t/state-variable-vs-ladder-filter/1704](https://forum.moogmusic.com/t/state-variable-vs-ladder-filter/1704)  
15. I'm not sure I understand what the difference is between different filters : r/edmproduction, accessed April 16, 2026, [https://www.reddit.com/r/edmproduction/comments/4j4f74/im\_not\_sure\_i\_understand\_what\_the\_difference\_is/](https://www.reddit.com/r/edmproduction/comments/4j4f74/im_not_sure_i_understand_what_the_difference_is/)  
16. 5 Use Cases for the New Auto Filter (+ Free Presets) \- Sonic Bloom, accessed April 16, 2026, [https://sonicbloom.net/auto-filter/](https://sonicbloom.net/auto-filter/)  
17. Ableton's Auto Filter— Comparing Circuit Models \- Reddit, accessed April 16, 2026, [https://www.reddit.com/r/ableton/comments/liudzd/abletons\_auto\_filter\_comparing\_circuit\_models/](https://www.reddit.com/r/ableton/comments/liudzd/abletons_auto_filter_comparing_circuit_models/)  
18. Ableton Wavetable: MS2 Filter Circuit \- YouTube, accessed April 16, 2026, [https://www.youtube.com/watch?v=FshC49pWqoQ](https://www.youtube.com/watch?v=FshC49pWqoQ)  
19. Sallen-Key Filter : Circuit, Working & Its Applications \- ElProCus, accessed April 16, 2026, [https://www.elprocus.com/sallen-key-filter/](https://www.elprocus.com/sallen-key-filter/)  
20. Sallen and Key Filter Design for Second Order Filters \- Electronics Tutorials, accessed April 16, 2026, [https://www.electronics-tutorials.ws/filter/sallen-key-filter.html](https://www.electronics-tutorials.ws/filter/sallen-key-filter.html)  
21. The Drop \- Cytomic, accessed April 16, 2026, [https://cytomic.com/product/drop/](https://cytomic.com/product/drop/)  
22. Sallen-Key Filters | mbedded.ninja, accessed April 16, 2026, [https://blog.mbedded.ninja/electronics/circuit-design/analogue-filters/sallen-key-filters/](https://blog.mbedded.ninja/electronics/circuit-design/analogue-filters/sallen-key-filters/)  
23. Cytomic Drops a new filter into The Drop \- Welcome, Prophet, accessed April 16, 2026, [https://cytomic.com/cytomic-drops-a-new-filter-into-the-drop/](https://cytomic.com/cytomic-drops-a-new-filter-into-the-drop/)  
24. If you've ever wondered how a diode ladder VCF (steiner-parker, TB-303, EMS) works, here's a layman-friendly video I did on how to design one from scratch. : r/synthdiy \- Reddit, accessed April 16, 2026, [https://www.reddit.com/r/synthdiy/comments/m1614h/if\_youve\_ever\_wondered\_how\_a\_diode\_ladder\_vcf/](https://www.reddit.com/r/synthdiy/comments/m1614h/if_youve_ever_wondered_how_a_diode_ladder_vcf/)  
25. Issue on implementation of Moog Ladder filter using ZDF \- DSP and Plugin Development Forum \- KVR Audio, accessed April 16, 2026, [https://www.kvraudio.com/forum/viewtopic.php?t=571909](https://www.kvraudio.com/forum/viewtopic.php?t=571909)  
26. Diode ladder filter \- Page 2 \- DSP and Plugin Development Forum \- KVR Audio, accessed April 16, 2026, [https://www.kvraudio.com/forum/viewtopic.php?t=346155\&start=15](https://www.kvraudio.com/forum/viewtopic.php?t=346155&start=15)  
27. How to use the Ableton Live AUTO FILTER audio effect \- PCAudioLabs, accessed April 16, 2026, [https://pcaudiolabs.com/how-to-use-the-ableton-live-auto-filter-audio-effect/](https://pcaudiolabs.com/how-to-use-the-ableton-live-auto-filter-audio-effect/)  
28. Cytomic SVF implementation in C++ \- GitHub Gist, accessed April 16, 2026, [https://gist.github.com/hollance/2891d89c57adc71d9560bcf0e1e55c4b](https://gist.github.com/hollance/2891d89c57adc71d9560bcf0e1e55c4b)  
29. Linear Trapezoidal Integrated State Variable Filter With Low Noise Optimisation \- Cytomic, accessed April 16, 2026, [https://www.cytomic.com/files/dsp/SvfLinearTrapOptimised.pdf](https://www.cytomic.com/files/dsp/SvfLinearTrapOptimised.pdf)  
30. Difference between filters in serial and parallel \- Signal Processing ..., accessed April 16, 2026, [https://dsp.stackexchange.com/questions/73658/difference-between-filters-in-serial-and-parallel](https://dsp.stackexchange.com/questions/73658/difference-between-filters-in-serial-and-parallel)  
31. Pole Mixing and Mutable Instruments Ripples (2020) clone \- VCV Community, accessed April 16, 2026, [https://community.vcvrack.com/t/pole-mixing-and-mutable-instruments-ripples-2020-clone/24398](https://community.vcvrack.com/t/pole-mixing-and-mutable-instruments-ripples-2020-clone/24398)  
32. Series and Parallel Filter Sections | Introduction to Digital Filters \- DSPRelated.com, accessed April 16, 2026, [https://www.dsprelated.com/freebooks/filters/Series\_Parallel\_Filter\_Sections.html](https://www.dsprelated.com/freebooks/filters/Series_Parallel_Filter_Sections.html)  
33. Epoch Modular Twin Peak MK2 2018 \- Reverb, accessed April 16, 2026, [https://reverb.com/item/19505675-epoch-modular-twin-peak-mk2-2018](https://reverb.com/item/19505675-epoch-modular-twin-peak-mk2-2018)  
34. Europa Shapeshifting Synthesizer \- Reason Studios, accessed April 16, 2026, [https://docs.reasonstudios.com/reason12/europa-shapeshifting-synthesizer](https://docs.reasonstudios.com/reason12/europa-shapeshifting-synthesizer)  
35. CellaVCV/docs/Cella\_Manual.md at main \- GitHub, accessed April 16, 2026, [https://github.com/victorkashirin/CellaVCV/blob/main/docs/Cella\_Manual.md](https://github.com/victorkashirin/CellaVCV/blob/main/docs/Cella_Manual.md)  
36. Best Eurorack Filters \+ Effects of 2019 \- Perfect Circuit, accessed April 16, 2026, [https://www.perfectcircuit.com/signal/best-eurorack-filters-effects-2019](https://www.perfectcircuit.com/signal/best-eurorack-filters-effects-2019)  
37. CellaVCV/README.md at main \- GitHub, accessed April 16, 2026, [https://github.com/victorkashirin/CellaVCV/blob/main/README.md](https://github.com/victorkashirin/CellaVCV/blob/main/README.md)  
38. Voltage Standards \- VCV Rack Manual, accessed April 16, 2026, [https://vcvrack.com/manual/VoltageStandards](https://vcvrack.com/manual/VoltageStandards)  
39. What's the point of writing "manual SIMD code"? \- Development \- VCV Community, accessed April 16, 2026, [https://community.vcvrack.com/t/whats-the-point-of-writing-manual-simd-code/3774](https://community.vcvrack.com/t/whats-the-point-of-writing-manual-simd-code/3774)  
40. rack::simd Namespace Reference \- VCV Rack API, accessed April 16, 2026, [https://vcvrack.com/docs-v2/namespacerack\_1\_1simd](https://vcvrack.com/docs-v2/namespacerack_1_1simd)  
41. XSIMD \- Development \- VCV Community, accessed April 16, 2026, [https://community.vcvrack.com/t/xsimd/22824](https://community.vcvrack.com/t/xsimd/22824)  
42. What's the point of writing "manual SIMD code"? \- Page 2 \- Development \- VCV Community, accessed April 16, 2026, [https://community.vcvrack.com/t/whats-the-point-of-writing-manual-simd-code/3774?page=2](https://community.vcvrack.com/t/whats-the-point-of-writing-manual-simd-code/3774?page=2)  
43. Resonant Filters | Filters | Electronics Textbook \- All About Circuits, accessed April 16, 2026, [https://www.allaboutcircuits.com/textbook/alternating-current/chpt-8/resonant-filters/](https://www.allaboutcircuits.com/textbook/alternating-current/chpt-8/resonant-filters/)  
44. rack::dsp Namespace Reference \- VCV Rack API, accessed April 16, 2026, [https://vcvrack.com/docs-v2/namespacerack\_1\_1dsp](https://vcvrack.com/docs-v2/namespacerack_1_1dsp)  
45. 1V/Oct, Hz/Volt : Synthesizer Tuning Standards Explained \- Perfect Circuit, accessed April 16, 2026, [https://www.perfectcircuit.com/signal/synthesizer-tuning-standards](https://www.perfectcircuit.com/signal/synthesizer-tuning-standards)  
46. Free Modules \- VCV Rack, accessed April 16, 2026, [https://vcvrack.com/Free](https://vcvrack.com/Free)  
47. Best way to set 1V/Oct CV out? \- Development \- VCV Community, accessed April 16, 2026, [https://community.vcvrack.com/t/best-way-to-set-1v-oct-cv-out/16438](https://community.vcvrack.com/t/best-way-to-set-1v-oct-cv-out/16438)  
48. Pro Modules \- VCV Rack, accessed April 16, 2026, [https://vcvrack.com/Pro](https://vcvrack.com/Pro)  
49. New Auto Filter Deep Dive \- Ableton 12.2 Update \- Reddit, accessed April 16, 2026, [https://www.reddit.com/r/ableton/comments/1jacwn1/new\_auto\_filter\_deep\_dive\_ableton\_122\_update/](https://www.reddit.com/r/ableton/comments/1jacwn1/new_auto_filter_deep_dive_ableton_122_update/)  
50. How To Use The Ableton Live AUTO FILTER \- OBEDIA | Music Recording Software Training And Support For Home Studio, accessed April 16, 2026, [https://obedia.com/how-to-use-the-ableton-live-auto-filter/](https://obedia.com/how-to-use-the-ableton-live-auto-filter/)  
51. AutoFilter \- I am confused here. Can someone tell me, given the settings in the image, is the filter actually doing anything. I thought that for the LFO to have an effect, that the "Amount" would have to be set to something other than zero. : r/ableton \- Reddit, accessed April 16, 2026, [https://www.reddit.com/r/ableton/comments/1c2la1v/autofilter\_i\_am\_confused\_here\_can\_someone\_tell\_me/](https://www.reddit.com/r/ableton/comments/1c2la1v/autofilter_i_am_confused_here_can_someone_tell_me/)  
52. All about the new Autofilter in Ableton Live \- 12.2 \- YouTube, accessed April 16, 2026, [https://www.youtube.com/watch?v=K7wdWFkI5RM](https://www.youtube.com/watch?v=K7wdWFkI5RM)  
53. Buy Eurorack synth modules \- Juno Records, accessed April 16, 2026, [https://www.juno.co.uk/dj-equipment/synth-modules/4/](https://www.juno.co.uk/dj-equipment/synth-modules/4/)