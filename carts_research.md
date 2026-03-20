# Turntable Cartridge Research: Scratch & Battle Carts

This document details the most popular cartridges used by scratch DJs and turntablists. It highlights why these specific models are chosen and how their mechanical design influences their "character" and performance.

---

## 1. Shure M44-7 (The Industry Legend)
*Status: Discontinued in 2018, but still the benchmark.*

*   **Character:** Heavy bass, extremely loud (9.5 mV), and punchy.
*   **Visual Character:** High-contrast "Panda" look (Matte Black body with Bright White stylus).
*   **Color Palette:**
    *   **Body (Black):** `rgb(26, 26, 26)` / `#1A1A1A`
    *   **Stylus (White):** `rgb(255, 255, 255)` / `#FFFFFF`
*   **Why Scratch DJs Pick It:** 
    *   **Skip Resistance:** Known as the "needle that never skips." It was engineered to stick in the groove even during violent scratch techniques.
    *   **Low Record Wear:** Despite its tracking ability, it uses a very light tracking force (1.5g – 3.0g), preserving the life of the vinyl during long practice sessions.
    *   **Sonic Identity:** It has a "vintage" warmth and a massive low-end that makes scratch samples sound "larger than life."
*   **DSP Implementation Notes:**
    *   **EQ:** A gentle "smile" curve with a significant low-shelf boost (approx +2-3dB) centered around 80Hz. A slight dip in the high-mids (3kHz) to reduce "harshness" during fast scratches.
    *   **Saturation:** Subtle even-order harmonics (warmth) to emulate its "Moving Magnet" character.
    *   **Dynamics:** A fast, soft-knee limiter or clipper to handle the high 9.5mV output without digital clipping, giving it that "fat" compressed feel.

## 2. Ortofon Concorde MKII Scratch
*Status: Current professional standard.*

*   **Character:** Bright, aggressive, and exceptionally loud (10.0 mV).
*   **Visual Character:** Modern and clean (Cool White body with Vibrant Red accents).
*   **Color Palette:**
    *   **Body (White):** `rgb(242, 242, 242)` / `#F2F2F2`
    *   **Accent (Red):** `rgb(230, 0, 0)` / `#E60000`
*   **Why Scratch DJs Pick It:** 
    *   **High Output:** With 10 mV of output, it provides a very hot signal that cuts through a mix, perfect for battle DJs.
    *   **Concorde Design:** The integrated "one-piece" headshell/cartridge design eliminates the need for wire mounting and alignment, making it robust for travel and quick setup.
    *   **Tracking:** Designed specifically for high tracking stability at higher forces (4.0g), ensuring it stays locked during fast back-and-forth movement.
*   **DSP Implementation Notes:**
    *   **EQ:** A high-shelf boost (+3dB) starting around 6kHz for "air" and bite. Very flat low-end (neutral).
    *   **Saturation:** Heavier "transistor-style" saturation to emulate the extremely high output driving a mixer's preamp. Focus on odd-order harmonics for "edge."
    *   **Physical Modeling:** Model a stiffer "suspension" (less phase-shift in the low-end during rapid direction changes).

## 3. Stanton 680 HP
*Status: A classic "New York" club and scratch favorite.*

*   **Character:** Warm mid-range, smooth highs, and a "fat" bottom end.
*   **Visual Character:** Industrial/Metallic (Polished Silver body with Cream stylus).
*   **Color Palette:**
    *   **Body (Silver):** `rgb(192, 192, 192)` / `#C0C0C0`
    *   **Stylus (Cream):** `rgb(255, 253, 208)` / `#FFFDD0`
*   **Why Scratch DJs Pick It:**
    *   **The "Sound":** Favored by DJs who prioritize the audio quality of the record as much as the scratch performance. It uses "Moving Iron" technology which many find more musical than standard "Moving Magnet" designs.
    *   **Durability:** Built like a tank. It was the standard in many high-end NYC clubs because it could handle both back-cueing and high-fidelity playback.
*   **DSP Implementation Notes:**
    *   **EQ:** A broad, gentle mid-range boost (400Hz - 1.2kHz) to add "body." A smooth high-frequency roll-off starting at 12kHz to emulate the "silky" highs.
    *   **Saturation:** Very clean, but with a unique "intermodulation" character (Moving Iron). Perhaps use a subtle parallel saturation stage.
    *   **Character:** Slightly "wider" stereo image (crosstalk modeling) to emulate the club/hi-fi heritage.

## 4. Ortofon Q.Bert (The Specialist)
*Status: Signature model developed with DJ Q.Bert.*

*   **Character:** Highly accentuated mid-range frequencies.
*   **Visual Character:** "The Ghost" (All White body with Jet Black graphics).
*   **Color Palette:**
    *   **Body (White):** `rgb(255, 255, 255)` / `#FFFFFF`
    *   **Graphics (Black):** `rgb(0, 0, 0)` / `#000000`
*   **Why Scratch DJs Pick It:**
    *   **Scratch Articulation:** By boosting the frequencies where scratch "vowels" (ahh, fresh) sit, it makes the scratching sound more defined and "talkative."
    *   **Ultra-High Tracking:** Specifically tuned for the most extreme turntablist techniques.
*   **DSP Implementation Notes:**
    *   **EQ:** A resonant peak (peaking filter) around 2.5kHz - 3kHz with a medium Q. This emphasizes the "vocal" quality of common scratch samples.
    *   **Saturation:** Sharp, aggressive clipping on transients to emphasize the "click" of the crossfader and the "bite" of the scratch.

---

## Technical Features Table

| Feature | Shure M44-7 | Ortofon MKII Scratch | Stanton 680 HP | Ortofon Q.Bert |
| :--- | :--- | :--- | :--- | :--- |
| **Primary Color** | Matte Black | Cool White | Polished Silver | White |
| **Stylus Tip** | Spherical (0.7 mil) | Spherical | Spherical | Spherical |
| **Output Voltage** | 9.5 mV | 10.0 mV | 8.0 mV | 11.0 mV |
| **Tracking Force** | 1.5g – 3.0g | 3.0g – 5.0g | 2.0g – 5.0g | 3.0g – 4.0g |
| **Best For** | Battle / Practice | Professional Gigs | Club / Sound Quality | Articulation |

---

## General Turntable Artifact Modeling

Beyond individual cartridge EQ/Saturation, a high-quality turntable model should include:

### 1. Stylus Friction (The "Grind")
- **Mechanism:** As the record moves under the needle, friction creates a broadband noise (surface noise).
- **DSP:** A low-level "pink noise" source modulated by the absolute playback velocity. When the speed is 0, the noise stops. As speed increases, the noise amplitude and "brightness" (LP filter cutoff) increase.

### 2. Tracking Distortion
- **Mechanism:** Inner-groove distortion occurs as the stylus approaches the center of the record.
- **DSP:** A variable saturation/LP-filter that increases as the `readHead` (position) moves closer to the virtual "center" of the buffer.

### 3. Harmonic Wobble (Wow/Flutter)
- **Mechanism:** Uneven platter rotation or off-center spindle holes.
- **DSP:** A low-frequency (0.5Hz - 4Hz) LFO modulating the `speed` variable. The "Battle" carts (M44-7, Concorde) should have *less* of this due to high-torque direct-drive motors, while "Vintage" modes could have more.

### 4. Directional Transients
- **Mechanism:** The physical cantilever "snaps" slightly when reversing direction.
- **DSP:** An envelope follower on the velocity delta. When direction flips, trigger a very short (5ms) impulse or high-pass "pop" to emulate the physical movement of the stylus in the groove.
