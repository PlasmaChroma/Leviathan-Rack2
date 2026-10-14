# Plasma conduit rollout

Functional connector guides use one of two group-name conventions in editable
panel masters:

- `plasma_conduit_candidates...` keeps the existing SVG strokes visible. These
  groups are classified and ready for a future conversion, but have no runtime
  behavior.
- `plasma_conduit_anchors` is runtime geometry. The split-panel pipeline hides
  this exact group in the generated panel, and `createPlasmaConduitLayer()` draws
  the cached native conduit treatment in its place.

Do not classify title rules, section-field outlines, glass silhouettes, or other
decorative purple geometry as conduit candidates.

## Current inventory

| Master | State | Paths | Functional guides |
| --- | --- | ---: | --- |
| `res/bifurx.svg` | Runtime | 5 | `fm_line`, `res_line`, `freq_line`, `bal_line`, `span_line` |
| `res/flux.svg` | Runtime | 8 | `gen1_attenuvert_line`, `gen4_attenuvert_line`, `surge_connector`, `sink_connector`, `sink_connector_4`, `surge_connector_4`, `input_2_line`, `input_3_line` |
| `res/undertow.svg` | Runtime | 4 | `freq_line`, `fine_freq_line`, `morph_line`, `lin_fm_line` |
| `res/deck.svg` | Runtime | 5 | `rate_connector-8`, `rate_connector`, `freeze_connector`, `gate_and_pos_connector`, `rev_and_rev` |
| `res/proc.svg` | Runtime | 2 | `surge_connector`, `sink_connector` |
| `res/wyrm.svg` | Runtime | 6 | `sync_line`, `freq_purple_line`, `slither_amp_line`, `slither_speed_line`, `FM_purple_line`, `fold_line` |
| `res/iris.svg` | Runtime | 4 | `iris_freq_guide`, `iris_scan_guide`, `iris_fm_guide`, `iris_soft_sync_guide` |

The remaining panel masters were reviewed and do not currently contain clear
functional purple connector strokes. In particular, `top_horizontal_line`
paths are decorative title rules. Purple geometry named `inputs`, `glass`,
`*_input_field`, or `*_section_fields` defines panel regions rather than a
control-to-jack relationship.

`res/proc.svg` also contains the hidden white `linear_line`. It is legacy art,
not part of the candidate group, and must not appear when Proc is converted.

## Converting a candidate panel

1. Confirm every candidate still represents a control-to-jack relationship.
2. Consolidate its candidate groups into one exact `plasma_conduit_anchors`
   group. Preserve the path IDs and geometry.
3. Add `createPlasmaConduitLayer()` to the module widget at the correct panel
   layer depth.
4. Regenerate split assets with
   `python3 tools/split_svg_labels.py res/<Module>.svg --overwrite`.
5. Run `make generate-panel-anchor-atlas` and the panel SVG contract tests.
6. Visually verify endpoints, overlays, screens, and controls in Rack before
   removing any legacy fallback treatment.

All currently classified guides are runtime straight two-point paths supported
by the shared renderer. New candidates should remain visibly authored until
their geometry and functional meaning have been reviewed.
