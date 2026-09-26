# MG204 reverse-engineering bundle

Start with `MG204_REVERSE_ENGINEERING.md`.

`MORPH_PARAMETER_NOTES.md` contains the recovered 22-stage rational Morph
schedule, upper-regime thresholds, and Morph Chord voice behavior.

To reproduce the firmware image from Make Noise's `mg204.wav`:

```bash
python3 -m pip install numpy
python3 decode_mg204.py mg204.wav
```

To verify/export the Gene Size curve for a splice of a chosen duration:

```bash
python3 extract_gene_size.py mg204_flash_08020000.bin \
  --splice-seconds 10 --csv gene_size_curve_10s.csv
```

The decompiler outputs are included as navigation aids. Their generated types
and expressions are not authoritative; use the address-level findings in the
report and the original Thumb instructions for exact work.
