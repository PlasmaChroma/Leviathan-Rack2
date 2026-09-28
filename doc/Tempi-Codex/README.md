# TEMPI 71 for Leviathan — implementation brief

This package specifies a faithful TEMPI 71 behavioral recreation inside the Leviathan VCV Rack plugin, with a Rack-native interface and a mandatory timing visualization. It specifies future work; it is not an implemented module or a claim of hardware equivalence.

- [Implementation specification](Leviathan_Tempi_Codex_Spec.md): behavior, fidelity boundaries, Rack adaptation, UI, architecture, persistence, implementation gates, and acceptance criteria.
- [Codex starting instruction](CODEX_START.md): ready-to-use implementation handoff.
- [Firmware behavioral source](../../firmware/Tempi/TEMPI71_BEHAVIORAL_ENGINEERING_SPEC.md): primary source, qualified by its evidence levels and the more detailed recovered models.
- [Leviathan development standard](../Leviathan-Standard.md): shared plugin conventions.

`Tempi` is a working implementation name. Final Leviathan product name, panel artwork, and model slug remain product decisions; behavior and visualization work need not wait for them. The proposed panel is 24 HP, subject to visual review. Do not copy the hardware panel as a prerequisite for faithful behavior.

The central distinction is between **verified digital behavior**, **documented musical behavior**, and **explicit software choices where reconstruction is incomplete**. The last category must remain visible in implementation status and release notes. Human Programming and live timing transitions are required work, not optional omissions hidden behind a working clock divider.
