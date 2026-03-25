# Translating Turntablist Scratches to TemporalDeck

Turntablism is the art of manipulating sounds and creating music using phonograph turntables and a DJ mixer. The core of scratching relies on the coordination between two hands:
1. **The Record Hand:** Moves the record forward and backward to alter the pitch and direction of the sound.
2. **The Fader Hand:** Uses the mixer's crossfader to cut the sound in and out, creating rhythmic patterns.

In the modular environment of VCV Rack, **TemporalDeck** acts as the turntable. The "Record Hand" is emulated by either dragging the UI platter or by patching modulation into the `POSITION_CV_INPUT`. The "Fader Hand" is typically emulated by patching TemporalDeck's audio outputs into a **VCA** and gating it with envelopes, triggers, or burst generators.

---

## The Live Buffer Challenge: Managing "Lag"

Unlike a physical vinyl record where a sound is locked in a static physical location, TemporalDeck captures audio into a **live, continuously filling circular buffer**. This means the "NOW" point (the write head) is constantly moving forward in time at audio rate. 

If you perform a perfectly symmetrical scratch (moving back 1 second, then forward 1 second), you do not return to the "NOW" point. Because time continued to pass while you were scratching, you will end up further behind in the buffer. Over time, perfectly symmetrical scratching causes the playhead to drift deeper into the past—this is called **Lag**.

To compensate for this when scratching live audio, you must adapt your techniques:

1. **Net-Forward Bias:** Your forward scratch movements (or forward CV envelopes) must be slightly faster or travel further than your backward movements to "catch up" with the advancing write head.
2. **The Freeze Function:** By engaging the `FREEZE` button (or `FREEZE_GATE`), you stop the write head from advancing. The buffer becomes a static piece of "vinyl," allowing you to execute traditional, perfectly symmetrical scratches without drifting into the past.
3. **Slip Mode Returns:** When you release a manual scratch or drop the `SCRATCH_GATE`, engaging **Slip Mode** tells the playhead to automatically snap or glide back to the "NOW" position, keeping your performance perfectly locked to the live incoming audio feed between scratch phrases.

---

## 1. The Baby Scratch
The foundation of all scratching. It involves simply moving the record forward and backward without using the crossfader at all.
* **Turntable Execution:** Fader remains open. The hand pushes the record forward, then pulls it back.
* **TemporalDeck Translation:** 
  * **With Freeze ON:** Patch a standard Triangle or Sine LFO into `POSITION_CV_INPUT`. The scratch stays perfectly centered on the frozen audio.
  * **Live (Freeze OFF):** Use an asymmetrical LFO (like a sawtooth or heavily skewed triangle) where the "forward" slope is slightly steeper to counteract buffer drift, or simply accept the drift and use a periodic trigger to snap the position back to 0.

## 2. The Forward Scratch (Drop / Release)
This scratch only lets the listener hear the forward motion of the record. The backward pull is muted.
* **Turntable Execution:** Start with the fader closed. Push the record forward and open the fader simultaneously. Close the fader, then pull the record back.
* **TemporalDeck Translation:**
  * **CV Control:** Patch an Attack-Decay Envelope into `POSITION_CV_INPUT`. To combat lag on a live stream, ensure the envelope's peak voltage is high enough to push the playhead fast enough to cover the distance of the pull-back *plus* the time elapsed. Gate a VCA with the same trigger so audio only passes during the forward push.

## 3. The Chirp Scratch
A classic, vocal-like scratch.
* **Turntable Execution:** Start with the fader open. Push the record forward, closing the fader right at the end of the forward stroke. Then, pull the record back, opening the fader right at the end of the backward stroke.
* **TemporalDeck Translation:**
  * **CV Control:** Use a Triangle LFO for the `POSITION_CV_INPUT`. Use a Square wave LFO for your VCA gate, slightly out of phase with the Triangle LFO so that the VCA cuts the audio right as the direction changes. *Tip: If scratching a live buffer, use Slip Mode so that when you stop the LFO, the audio seamlessly returns to the live input.*

## 4. The Transformer Scratch
A rhythmic, stuttering sound created by cutting a long sound into smaller pieces.
* **Turntable Execution:** The record hand slowly drags the sample in one direction (usually forward). The fader hand starts closed and rapidly taps the fader open and closed.
* **TemporalDeck Translation:**
  * **CV Control:** Patch a slow Ramp (Sawtooth) LFO into `POSITION_CV_INPUT`. If the buffer is live, a slow ramp *forward* actually just slows down playback (since the write head is moving away from you). To simulate a forward drag, your ramp needs to move *faster* than 1 Volt per second (depending on scaling). Patch a fast Square LFO or a sequenced gate pattern into your VCA to "transform" the sound.

## 5. The Flare (1-Click / 2-Click / Orbit)
Flares are an "open fader" technique. Instead of tapping the fader *on* to make sound, you tap it *off* to cut the sound, effectively splitting one continuous scratch into multiple notes.
* **Turntable Execution (1-Click Flare):** Fader is open. Push the record forward, and quickly close-then-open the fader exactly once in the middle of the stroke. This splits the forward sound into two notes.
* **TemporalDeck Translation:**
  * **CV Control:** Patch a Triangle LFO to the `POSITION_CV_INPUT`. Send a high/open voltage to your VCA, but use a short trigger/burst generator connected to an inverted VCA input (or ducking envelope) to rapidly "duck" or cut the audio briefly during the middle of the LFO's rise and fall.

## 6. The Crab Scratch
An incredibly fast iteration of the Flare/Transformer, utilizing the fingers as a spring mechanism against the fader.
* **Turntable Execution:** The DJ rubs 3 or 4 fingers rapidly across the fader against the thumb, creating 3 or 4 incredibly fast cuts while moving the record.
* **TemporalDeck Translation:**
  * **CV Control:** Move the platter manually or with a slow CV slew. For the crossfader cuts, use a **Burst Generator** module. Trigger the burst generator to fire 3 or 4 rapid gates into your VCA. This flawlessly mimics the multi-finger fader tap.

---

### Summary of Modular Patching Strategy

To accurately mimic a turntablist in VCV Rack with TemporalDeck:
1. **The Record Hand (`POSITION_CV_INPUT`):** Think of your modulation source as the physical hand. Slew limiters, smooth envelopes, and LFOs represent the momentum of a human arm. **Remember to account for the moving write-head when working with live audio!** Use asymmetrical modulation or the `FREEZE` function to control drift.
2. **The Fader Hand (VCA):** Run TemporalDeck's output through a VCA. Gate sequencers, burst generators, and square LFOs represent the sharp, binary cuts of a crossfader.
3. **The Engine:** Engage TemporalDeck's physical modeling algorithms (Cartridge selection, Scratch Interpolation) to ensure that the rapid pitch fluctuations and direction changes sound authentically mechanical and warm, rather than digital and sterile.