# Temporal Deck — Control & I/O Descriptions

## BUFFER

Sets the maximum accessible depth of recorded time.
Fully clockwise allows access to the entire 8-second memory. Turning counter-clockwise restricts how far into the past the platter may travel.

---

## RATE

Controls the speed at which the playback head moves toward the present moment.
Center position corresponds to normal time. Turning clockwise accelerates playback, while counter-clockwise slows the return toward NOW.

---

## MIX

Crossfades between the incoming signal and the processed Temporal Deck signal.
Fully counter-clockwise outputs the dry input. Fully clockwise outputs only the time-warped signal.

---

## FEEDBACK

Routes the Temporal Deck output back into the recording buffer.
Increasing feedback creates repeating textures and evolving loops within the time buffer.

---

# CV Inputs

## POSITION CV

Controls the position of the playback head within the recorded buffer.
External modulation may be used to scrub or automate movement through recorded time.

---

## RATE CV

Modulates the playback speed of the Temporal Deck transport.
Positive voltages increase speed toward the present moment, while negative voltages slow the playback head.

---

# Gate Inputs

## FREEZE GATE

Freezes the recording buffer.
When active, new audio is no longer written to the buffer, allowing the recorded material to be explored without change.

---

## SCRATCH GATE

Enables external control of scratching behavior.
When high, the playback head may be manipulated via POSITION CV without affecting the underlying timeline.

---

# Transport Buttons

## FREEZE

Stops new audio from entering the buffer.
The current contents of the Temporal Deck memory remain available for manipulation.

---

## REVERSE

Reverses the direction of playback within the buffer.
Temporal motion moves away from the present instead of toward it.

---

## SLIP

Allows temporary scratching without losing timeline alignment.
When released, playback returns smoothly to the current moment in time.

---

# Audio Inputs

## IN L

Left channel audio input for recording into the Temporal Deck buffer.

## IN R

Right channel audio input for recording into the Temporal Deck buffer.

---

# Audio Outputs

## OUT L

Left channel output of the Temporal Deck processor.

## OUT R

Right channel output of the Temporal Deck processor.

---

# Platter

## PLATTER

Interactive time surface.
Dragging the platter moves the playback head through recorded time, allowing scratching and temporal repositioning of audio.

---

# Arc Display

## ARC

Displays the temporal distance between the playback head and the present moment.
The arc expands as playback moves deeper into the past and collapses as the signal returns to NOW.

---

# Buffer Limit Indicator

## BUFFER LIMIT DOT

Indicates the maximum backward position allowed by the BUFFER control.
Also reflects the current fill state of the buffer during startup.

---

If you'd like, we can also write a **short “Patch Tip” section** (also very Make Noise style) that explains how someone might creatively use Temporal Deck in a patch.
