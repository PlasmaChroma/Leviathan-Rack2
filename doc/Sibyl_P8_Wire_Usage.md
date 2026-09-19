# Sibyl P8 wire usage

P8 extends control-side authoring and reads; saved compositions remain schema 4.
All edits still use canonical validation, revision guards, collision handling and
normal adoption rules. Existing clients retain full edit responses by default.

## Discovery and receipts

`GET /sibyl/{id}/capabilities?format=manifest` returns a small manifest with
`instance` and `fingerprint`. Fetch the full capabilities only when this pair
changes. The fingerprint excludes score revision; instance changes when the
module is recreated. `?topic=p8` returns supported compact features. Python/MCP
clients perform this negotiation automatically, without caching legacy contracts
that lack a server identity.

Include `response_profile: "receipt"` in an edit for aggregate counts, affected
objects and acceptance/adoption metadata. `summary` adds per-operation counts;
`full` retains detailed change reports. Errors and warnings remain intact in all
profiles. A replayed prepared handle returns its original receipt unchanged.

For a smaller MCP tool inventory, launch the server with
`OCTAVIA_SIBYL_TOOLSET=compact`. Use `vcv_sibyl_request` with a module ID, an action
and its parameters. Discover each action's typed input through
`vcv_sibyl_get_capabilities(topic="toolSchemas")`. Default registration remains
`full` for compatibility; the setting affects Sibyl tools only.

## Arithmetic batches and sparse exceptions

```json
{"op":"insert_note_batch","pattern_id":"p","encoding":"columns_v1","columns":["tuned.step"],"rows":[[0],[17]],"defaults":{"tuned":{"context":"c"},"velocity":0.7},"onsets":{"start":0,"spacing":2,"count":8},"overrides":{"7":{"tuned":{"context":"c","ratio":"14/8"},"gate":0}}}
```

The onset series supplies `step`; expression rows cycle until `count` events are
generated. `start` is 0–1023, `spacing` is 1–1024, and `count` is 1–1024. Every
generated step must fit the destination pattern. A `step` column cannot accompany
`onsets`. Zero columns with a single empty row can repeat defaults alone.

Expansion order is defaults, row values, then overrides. Override keys are
zero-based decimal row indices without leading zeroes; values use canonical event
fields. An override replaces the entire named field, including nested objects.
Null column cells omit a value; canonical overrides retain explicit null and are
subject to the ordinary validator. Nested defaults are copied independently.

`tools.sibyl_composer.expand_note_batch` provides the equivalent canonical
`insert_notes` form. The client automatically uses this fallback when the server
does not advertise the requested batch features. No expansion occurs in DSP.

## Compact reads

Use `view=arrangement` for scene assignments, overrides and referenced pattern
summaries without event arrays. Pages contain eight scenes by default, at most 32;
use `nextCursor` for continuation. Bounded collections report totals and omissions.

For the notes view, add `encoding=columns_v1`. The response replaces `notes` with
`columns`, `rows`, and `missing`. `missing` maps row indices to absent column
indices, distinguishing an absent field from an explicit null cell. Use
`decode_note_columns(response)` to reconstruct event objects. Existing field
projections and revision-bound pagination still apply; changing encoding requires
starting a new page sequence.

## Toolkit additions

```python
from tools.sibyl_composer import EdoTuning, PatternBuilder, VoicingBuilder

tuning_ops = EdoTuning(13, "3/1").to_operations("tritave", "c")
walk = PatternBuilder(pitch_context="c").arpeggiate(
    [0, 4, 7], contour="random_walk", seed=314, period_steps=13, octave_range=2)
voicing = VoicingBuilder("h", "s").spread({"bass": "bass_p", "lead": "lead_p"}, -1, 2)
```

Random walks require an explicit seed and do not alter Python's global random
state. Native integer pitches need explicit `period_steps` for multi-period
arpeggios. `VoicingBuilder` invokes the existing bounded native solver; default
`createOnly` needs unused patterns and unassigned destination tracks. Use explicit
`write_policy="replace"` to replace existing assignments. Scene automation uses
`AutomationBuilder(track, scope="scene", scene_id=...)`, with `sceneRepeat` or
`sceneVisit` clock. Inputs remain subject to the native musical and size limits.
