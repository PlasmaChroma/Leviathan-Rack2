# Temporal Deck — DSP Architecture Specification v1.0

```markdown id="temporal_deck_dsp_arch_v1"
MODULE: Temporal Deck
TYPE: Time-buffer / scratching engine
TARGET: VCV Rack 2
AUDIO: Stereo
BUFFER_LENGTH: 8 seconds (fixed)
INTERPOLATION: Cubic
```

---

# 1. Core Engine Overview

Temporal Deck continuously records incoming audio into a circular buffer while allowing the read head to move freely within recorded history.

The engine maintains three primary indices:

```text
writeHead  → current write position (NOW)
readHead   → playback position
oldestHead → oldest valid sample in buffer
```

Relationships:

```text
oldestHead ≤ readHead ≤ writeHead
```

Except during temporary scratch manipulation.

---

# 2. Buffer Structure

Buffer allocation occurs once during module initialization.

For a system sample rate `sr`:

```text
bufferSamples = sr * 8 seconds
```

Example at 48 kHz:

```text
bufferSamples = 384000
```

Buffer layout:

```text
struct BufferSample {
    float left;
    float right;
};
```

Storage:

```text
BufferSample buffer[bufferSamples];
```

---

# 3. Write Head Behavior

Each audio frame:

```text
buffer[writeHead] = inputSample
writeHead++
```

Wrap logic:

```text
writeHead = writeHead % bufferSamples
```

Oldest sample advances once the buffer is full:

```text
oldestHead = writeHead + 1
```

Adjusted for wrapping.

---

# 4. Read Head Behavior

The read head moves according to transport speed.

Base update:

```text
readHead += playbackSpeed
```

Where:

```text
playbackSpeed = rateKnob + rateCV
```

Clamp rule:

```text
readHead ≤ writeHead
```

If exceeded:

```text
readHead = writeHead
```

Meaning playback becomes live input.

---

# 5. Buffer Limit (BUFFER Knob)

BUFFER knob defines maximum lag:

```text
maxLagSamples = BUFFER * bufferSamples
```

Read head cannot move beyond:

```text
minRead = writeHead - maxLagSamples
```

Clamp rule:

```text
readHead ≥ minRead
```

---

# 6. Circular Index Handling

Indices must wrap safely.

Example helper:

```text
wrapIndex(i):
    while i < 0:
        i += bufferSamples
    while i ≥ bufferSamples:
        i -= bufferSamples
    return i
```

---

# 7. Interpolation

Since `readHead` is fractional, interpolation is required.

Chosen method:

```text
cubic interpolation
```

Using four samples:

```text
y0 = buffer[i-1]
y1 = buffer[i]
y2 = buffer[i+1]
y3 = buffer[i+2]
```

Fraction:

```text
t = readHead - floor(readHead)
```

Output sample computed via cubic polynomial.

This produces smooth pitch-shifted playback.

---

# 8. Platter Scratching

During platter interaction:

```text
readHead = platterPosition
```

Where platterPosition maps to:

```text
writeHead - lagFromGesture
```

Gesture input modifies `lagFromGesture`.

Platter smoothing:

```text
platterState += (targetState - platterState) * inertia
```

Read head follows `platterState`.

---

# 9. Slip Mode

Slip maintains a hidden timeline.

Variables:

```text
timelineHead
```

Timeline always advances with write head.

During scratching:

```text
readHead = platterPosition
timelineHead += playbackSpeed
```

On release:

```text
readHead → timelineHead
```

Smooth transition applied.

---

# 10. Freeze Mode

Freeze halts write head movement.

```text
freezeActive → stop writing new samples
```

Write head remains fixed.

Read head continues moving within frozen buffer.

---

# 11. Feedback

Feedback mixes playback output back into the buffer.

```text
feedbackSample = outputSample * feedbackAmount
buffer[writeHead] += feedbackSample
```

Feedback applied per channel.

Clipping protection recommended.

---

# 12. Output Signal Path

Dry/wet mixing:

```text
output = dry * (1 - mix) + wet * mix
```

Where:

```text
wet = interpolated buffer sample
```

---

# 13. Rate CV Scaling

Rate modulation:

```text
speed = rateKnob * exp(rateCV * scale)
```

Example mapping:

```text
±5V → 0.5× – 2× multiplier
```

This keeps modulation musically useful.

---

# 14. Lag Visualization Data

Lag calculation:

```text
lagSamples = writeHead - readHead
```

Arc ratio:

```text
lagRatio = lagSamples / maxLagSamples
```

Arc angle:

```text
arcAngle = lagRatio * 180°
```

---

# 15. Buffer Limit Marker

Limit marker position:

```text
limitRatio = maxLagSamples / bufferSamples
limitAngle = limitRatio * 180°
```

---

# 16. Performance Safeguards

### Prevent Denormals

Use small DC offset or flush-to-zero.

### Avoid clicks

Smooth transitions when:

* platter release
* slip return
* freeze toggling

---

# 17. CPU Expectations

Operations per sample:

* 1 buffer write
* 1 cubic interpolation
* minimal pointer math

Estimated CPU footprint:

```text
very low (<0.5% typical)
```

Even at high polyphony.

---

# 18. Future DSP Extensions

Possible improvements later:

* anti-alias filtering for extreme speeds
* higher-order interpolation
* granular freeze textures
* variable buffer sizes

---

# Result

With this DSP architecture spec and the earlier **Design Spec**, Temporal Deck now has:

* fully defined audio engine
* complete UI semantics
* interaction model
* visualization mapping
* code-ready structure

This combination should allow **direct implementation or Codex generation with minimal ambiguity**.

---

If you'd like, the next step that could really accelerate development would be creating a **small block diagram of the Temporal Deck signal flow**, which makes debugging and optimization much easier once coding begins.
