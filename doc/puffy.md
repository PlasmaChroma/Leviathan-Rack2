# Research brief for a VCV Rack puffer saturator limiter

## What sonible’s puffer:fish publicly tells you

From the public product page and manual, **puffer:fish is first and foremost a saturation plug-in with a character-driven UI**, not a public-facing limiter. Sonible describes it as a free saturation plug-in with **three “characters”** that react visually as you drive the effect harder, and the manual exposes a very small control set: **Puffiness** for the saturation amount, **Deflate** for output level compensation after the effect, **Character** for mode selection, plus bypass and settings. The three modes are described as **Tinyfin** for gentle warmth/cohesion, **Spikeskin** for harder-edged distortion, and **Twitchgill** for more input-reactive, chaotic behavior. Public docs describe the sound goals and UI behavior, but they do **not** disclose exact transfer functions, oversampling strategy, or limiter math. citeturn1view0turn3view0

That matters for your VCV Rack project, because it means a “comparable” module should be understood as **comparable in interaction design and personality**, not as a literal DSP clone. The public information is enough to infer a design language: a small number of controls, three tonal modes, aggressive visual feedback, and a mascot whose body language tracks the sound. It is **not** enough to reverse-engineer sonible’s internal algorithm with confidence. citeturn1view0turn3view0

There is one more useful clue in sonible’s own docs: puffer:fish lists **OpenGL 3.2+** as a GPU requirement, and its settings page explicitly says **OpenGL can cause rendering issues on certain hardware** and can be disabled. That strongly suggests the UI is graphically ambitious enough to justify hardware-accelerated rendering, while also reminding you that flashy visuals create compatibility risk. For a Rack module, that makes the visual ambition feasible, but it also argues for a conservative fallback path. citeturn1view0turn3view0

## What the saturation DSP should actually do

If the Rack module is meant to feel like “the cute fish that makes the master louder and hairier,” the DSP core should stay simple at the control layer and sophisticated under the hood. The right public mental model is **mode-dependent nonlinear waveshaping plus output compensation**. That aligns closely with sonible’s own control language: “Puffiness” increases harmonics and density, while “Deflate” compensates for level increase after saturation. citeturn3view0

The central technical issue is aliasing. VCV Rack’s own DSP documentation says anti-aliasing is generally required for **waveshaping, distortion, and saturation**, and identifies the general solution as **running the nonlinearity at a higher internal sample rate, low-pass filtering, and decimating back down**. Independent academic work from Aalto/Edinburgh likewise notes that soft-clipping algorithms are major sources of aliasing and that dedicated anti-aliasing strategies such as oversampling and antiderivative-based methods materially reduce those artifacts. citeturn8view0turn10search4turn10search13

For that reason, the cleanest architecture for a VCV module like this is:

**input trim → oversampled nonlinear stage → optional tone correction / DC block → output trim → limiter stage**

That order is not arbitrary. The true-peak and limiter literature consistently treats nonlinear, fast, or overshoot-inducing stages as things the limiter should catch **afterward**, not before. The AES streaming recommendations also note that filtering can itself create overshoot, so if you do include a DC blocker or any tonal post-filter, it should be accounted for **before** the final peak-control stage. citeturn23view0turn11search4turn12view1

A three-mode design inspired by puffer:fish makes excellent sense:

**Tinyfin** should be a mostly symmetrical, soft-knee saturator whose job is to add density and perceived loudness without shouting about itself. A normalized soft clipper in the tanh/arctan family is a practical fit here, because it gives smooth onset and predictable control. The manual’s language for Tinyfin is explicitly about warmth, body, cohesion, and a musical, controlled increase in harmonics. citeturn3view0

**Spikeskin** should be the “harder knee, faster harmonic stack” mode. Sonible describes it as edgy, aggressive, transient-sharpening distortion rather than merely warm saturation, so this is the place for a steeper transfer curve and a stronger drive-to-output-gain relationship. This mode is where higher oversampling is most justified, because sharper nonlinearities are more prone to objectionable aliasing. citeturn3view0turn8view0turn10search4

**Twitchgill** is the most interesting one, because sonible’s own description says it **reacts dynamically to the input signal** and becomes more unstable as it is pushed. That points toward a slightly stateful design rather than a purely static transfer curve. A good Rack interpretation would be an asymmetrical or envelope-modulated waveshaper whose shape shifts with recent signal level, crest factor, or transient energy. If you do that, you should budget for a **DC blocker**, because asymmetrical processing can introduce DC bias; FabFilter’s documentation explicitly notes that asymmetrical waveform processing or saturation can create DC offset and that filtering it out prevents unnecessary asymmetrical limiting downstream. citeturn3view0turn20view0turn20view1

A smart implementation detail is to make **output loudness honesty optional**. Sonible’s “Deflate” is explicitly there because saturation raises level. Your Rack version should offer both a manual output/ceiling control and an optional gain-compensation mode for fair A/B comparison. FabFilter calls this “Unity Gain” and recommends it because loudness increases make almost anything seem better at first listen. citeturn3view0turn20view0

## How to combine saturation and limiter without wrecking the stereo image

Your instinct to fuse a saturator and limiter into one end-of-chain module is good. In practice, that makes the device much closer to a **stereo finishing box** than to a simple distortion effect. The limiter should not replace the saturator; it should **govern the consequences** of the saturator. EBU, AES, and FabFilter all stress the same general point: fast nonlinear or limiting processes can produce **inter-sample or true peaks**, meaning the reconstructed analog waveform can exceed the sample peaks you see in the digital signal. Oversampling, true-peak metering, and lookahead exist precisely because naïve sample-peak limiting is not enough. citeturn13view0turn23view0turn11search4turn12view1

For an end-of-chain Rack module, a very sensible limiter split is:

- a **sample-peak “instant safety” stage** for low-latency use, and  
- an optional **lookahead / true-peak ceiling stage** for mastering-style use. citeturn21view0turn11search4turn23view0

That split respects two different use cases. A live patch or feedback-sensitive modular graph may not want much latency, while a final stereo output processor absolutely benefits from lookahead and true-peak protection. Signalsmith’s limiter walkthrough makes the underlying engineering point cleanly: a lookahead limiter works by generating a smooth gain envelope and **delaying the program signal** so the gain reduction can arrive slightly ahead of the peaks. SSL’s limiter guide and FabFilter’s manual both describe the same principle: lookahead catches peaks by introducing internal latency and giving the algorithm time to respond cleanly. citeturn21view0turn11search20turn12view1

For stereo handling, the public limiter literature gives a very useful design pattern. FabFilter explains that on stereo programs, it is often best to keep the **release stage strongly linked** to avoid image drift, while allowing the **fast transient stage** to be less than 100% linked so a one-sided click or spike does not unnecessarily pull down the whole stereo image. That is an excellent blueprint for your fish module: keep the image stable, but do not overreact to every tiny left-only or right-only transient. citeturn12view2turn12view3

So the recommended signal topology is:

**stereo input → drive stage → mode-specific saturator → DC blocker if needed → linked stereo limiter → output ceiling / makeup**

With that topology, your front panel can stay charmingly simple even though the internal machine is doing real work. A minimal but serious control set would be **Character**, **Amount**, **Ceiling/Deflate**, and a **CV input with attenuverter** for Amount. Attack, release, link percentage, oversampling factor, and true-peak mode can live in the context menu or be auto-tuned by Character. That lets the module preserve puffer:fish-like immediacy while still functioning as a genuine stereo finisher. citeturn3view0turn12view1turn12view2turn23view0

## How this fits into VCV Rack technically

At the Rack level, this is a very comfortable module to build. VCV’s development tutorial and API guide lay out the basic structure clearly: modules expose **params, inputs, outputs, and lights**, and then do actual audio work inside `process()`. For an effect module like this, bypass routing is also worth implementing so that when the user bypasses the module, left and right pass straight through. Rack explicitly supports that with `configBypass(LEFT_INPUT, LEFT_OUTPUT)` and `configBypass(RIGHT_INPUT, RIGHT_OUTPUT)`. citeturn0search1turn17view0

For stereo I/O, there are two Rack-friendly approaches. One is the obvious UI-first version: separate **L input, R input, L output, R output** jacks. The other is polyphonic handling under the hood. Rack’s voltage and polyphony standards recommend treating the channel count of a primary input as the number of active engines and copying monophonic modulation inputs across those engines with `getPolyVoltage(c)`. The API guide also shows how to process polyphonic signals one channel at a time or in **SIMD groups of four** using `simd::float_4`, and specifically notes that SIMD can help with stereo and quad processing too. That means you can present the module as a stereo processor while still writing the engine so it scales naturally to polyphonic or grouped processing. citeturn16view0turn15view0turn14search13

A practical compromise is this: **expose dedicated L/R jacks**, but if only the left input is connected and it carries two channels, treat it as a stereo poly cable. That is not mandated by Rack, but it harmonizes nicely with Rack’s polyphony conventions and modern user expectations. The main point is that the DSP core should already be channel-agnostic, because Rack’s own guidance strongly encourages modules to behave gracefully with poly inputs and to support up to sixteen channels where appropriate. citeturn16view0turn15view0

Rack’s own voltage guidance is also relevant to your CV story. Audio outputs are typically around **±5 V**, while modulation sources are commonly **0–10 V unipolar** or **±5 V bipolar**. That makes a straightforward control scheme easy: let the Amount knob define the base saturation, and let a CV jack with attenuverter apply either positive-only or bipolar modulation over it. For a performance-oriented module, 0–10 V can map cleanly from “no puff” to “full puff,” while bipolar CV can be allowed to push the mode between tamer and nastier internal operating regions. citeturn16view0

There are also two Rack-specific safety details worth keeping. First, VCV advises developers not to use crude hard clamping as a generic fallback and instead to avoid pathological outputs intelligently. Second, if a process can ever produce NaNs or infinities, the module should zero those out before they escape. Those are small details, but they matter in high-drive nonlinear code, especially once oversampling and asymmetry enter the room. citeturn16view0

## How to make the puffer character feel alive without turning the module into a GPU disaster

The mascot concept is not fluff; it is part of the product identity. Sonible explicitly sells puffer:fish on the idea that the characters **react visually as you push the saturation**, and that is almost certainly why the plug-in reads as memorable instead of just being “yet another saturator.” citeturn1view0

In Rack, the important distinction is between **audio-reactive motion** and **idle life**. Audio-reactive motion should communicate state: body inflation, cheek puff, spine extension, eye size, or mouth compression can all map to some smoothed combination of **Amount**, input level, harmonic intensity, or limiter gain reduction. Idle life should not depend on the audio at all: blinks, small glances, tiny breathing loops, and occasional micro-fidgets make the fish feel alert instead of dead. This is exactly aligned with your instinct that the eye blink / glance can be trivial and not tied to signal analysis.

Technically, Rack gives you several viable UI layers. The API guide explains that ordinary widget drawing happens every frame, and that **complex widgets can become expensive**, so cached drawing through a `FramebufferWidget` is recommended when you do not need to redraw continuously. The same guide also shows how to mark that framebuffer dirty only when needed. Separately, Rack’s API includes an `OpenGlWidget` that can draw into a framebuffer with OpenGL, specifically by overriding `drawFramebuffer()`. So yes: **true 3D or pseudo-3D rendering is possible**. citeturn15view0turn6search2turn6search7

But the engineering answer is more nuanced than “full 3D is possible, therefore do full 3D.” Sonible’s own puffer:fish manual literally includes a setting to disable OpenGL because it can cause rendering issues on some systems. Rack’s migration guide also warns that OpenGL-tied resources such as fonts and images should not be cached across window lifetimes because the window and GL context can be recreated. Taken together, those sources strongly suggest a layered strategy: **ship a robust 2D or pseudo-3D mascot first**, then only escalate to real 3D if the payoff is clearly worth the maintenance and compatibility cost. citeturn3view0turn17view0

In practice, the sweet spot is probably one of these two:

A **rigged 2D fish** built from vector or mesh-deformed parts, with body scale, fin bend, eye blink, outline squash, and subtle shading shifts. This is the lowest-risk route and will still look lively if animated well.

A **pre-rendered pseudo-3D fish** with a small number of angle states and morph targets, crossfaded or switched according to inflation and expression. This often delivers 80% of the charm of real 3D at a fraction of the implementation cost.

For animation scheduling, you do not need to redraw everything at audio rate. Rack’s DSP utilities include a `ClockDivider` for “every N process calls” timing, and the namespace includes a smoothed `VuMeter2`. That means you can update your analysis and visual state at a lower control rate, smooth it, and only mark the framebuffer dirty when something visible changes. If you want parts of the mascot to glow independently of room brightness, Rack also supports drawing on the self-illuminating layer via `drawLayer()`. citeturn9view2turn9view3turn15view0turn17view0

## What I would actually build

If the goal is a module people will really use—not just admire once and forget—I would build it as a **stereo end-of-chain saturator/limiter with a character-first UI and mastering-safe defaults**.

The first release should center on three user-facing behaviors directly inspired by puffer:fish: a gentle “glue” mode, an aggressive “bite” mode, and a dynamic “chaos” mode. Public sonible materials already frame those tonal buckets clearly, and they map beautifully onto three internally distinct nonlinear strategies. citeturn1view0turn3view0

The front panel should stay sparse: **Character**, **Amount**, **Ceiling/Deflate**, an **Amount CV jack** with attenuverter, and stereo I/O. Add a little gain-reduction or “panic blush” indicator when the limiter is working. If you want one extra control, make it **Link** or **Tone**, but only if testing proves it solves a real need. Puffer:fish’s public design works precisely because it does not bury the user in knobs. citeturn3view0

Internally, I would default to **4x oversampling** for the nonlinear stage, because VCV’s DSP notes treat oversampling/decimation as the standard general anti-aliasing path for nonlinear processing, and FabFilter’s limiter documentation repeatedly points to 4x oversampling as a strong practical baseline for controlling aliasing and inter-sample behavior. For the nastiest mode, I would allow **8x** in the context menu. If CPU becomes an issue, ADAA-style antialiasing becomes a legitimate second-phase optimization. citeturn8view0turn12view1turn10search13

For the limiter, I would ship two operating styles behind the same cute face: an **Eco** mode with minimal latency and simple sample-peak safety, and a **Master** mode with lookahead and optional true-peak control. EBU and AES both make clear that true-peak limiting and oversampled measurement are the right tools when the goal is preventing reconstruction overshoots, especially at the very end of a chain. citeturn13view0turn23view0turn11search4

If you want a musically sensible factory default for the Master mode, the public standards and manuals point to a very reasonable cluster: moderate lookahead, linked stereo release, partially linked transient handling, and a conservative ceiling. EBU production guidance uses **−1 dBTP** as its recommended maximum true-peak level in production, while AES notes that downstream codecs and SRC can push needed headroom even lower in some delivery chains. For Rack users not targeting broadcast delivery, the exact ceiling can remain a creative choice, but making **true-peak-safe behavior easy** is still a strong product decision. citeturn13view0turn23view0

And for the fish itself: I would not overcomplicate the first version. Make it breathe. Make it blink. Make it puff based on a smoothed blend of **Amount**, post-drive level, and limiter gain reduction. Let Spikeskin show more spines as the limiter starts shaving peaks. Let Twitchgill’s eyes dart a little when the input turns unstable. Give Tinyfin the calm, smug little face of a fish that knows the mix is now 7% more expensive. The public sonible concept proves that users respond to personality when it is tied to audible consequence. The trick is not the mascot alone. The trick is that the mascot becomes a readable meter. citeturn1view0turn3view0turn15view0

## Bottom line

The strongest version of this idea is **not** “copy puffer:fish into Rack.” It is: **build a Rack-native stereo finishing module that borrows puffer:fish’s emotional design language, then deepens it with a serious anti-aliased saturation core and a genuinely useful limiter stage**. Public information supports the mascot-driven, three-character saturation concept very well, but it does not reveal sonible’s internal DSP, so you are free to make the Rack version more purpose-built for modular end-of-chain use. citeturn1view0turn3view0

If I were choosing the engineering priorities, I would rank them this way: **sound first, aliasing control second, stereo image preservation third, animation fourth, true 3D last**. Rack’s APIs absolutely let you do the visuals, but the sources point to a clear truth: nonlinear audio quality and peak handling determine whether the module becomes a toy or a keeper, while the fish decides whether it becomes beloved. citeturn8view0turn15view0turn6search2turn3view0