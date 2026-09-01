# PHONEX corpus provenance

PHONEX speech data is a clean-room, synthetic Leviathan work. It does not use,
transform, or require dumped commercial speech ROMs, proprietary recordings,
pronunciation corpora, TTS engines, or network services.

The Phase 4 prototypes are authored from synthetic formant targets and simple
reflection-coefficient shapes. The generator derives order-10 LPC reflection
coefficients from five pole-pair formants without consulting recordings or a
third-party speech corpus. All bundled pronunciations are Leviathan-authored.

At sequence compilation time, the authored parameters are reconstructed through
the publicly documented TMS5100 energy, pitch, and reflection-coefficient lookup
tables. These functional chip tables contain no vocabulary or speech-ROM data.
