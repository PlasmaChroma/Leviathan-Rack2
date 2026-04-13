# Crownstep Melodic Bias

## Purpose

Crownstep currently derives pitch primarily from board geometry and move interpretation:

1. move history
2. source / destination / blend selection
3. board layout / divider / bipolar mapping
4. quantizer / key / scale interpretation
5. root / transpose offset
6. output

This produces flexible results, but it does not inherently optimize for melodic continuity or phrase quality.

A melodic layer can sit on top of the existing pitch derivation and improve musicality without changing the underlying game logic, move history, or sequencing model.

## Core Design Principle

The melodic layer should be an interpretation stage, not a rewrite of the stored sequence.

That means:
- move history remains the source of truth
- board-derived pitch remains available unchanged
- melodic shaping is applied at playback/output derivation time
- the user can toggle it on or off and hear the difference immediately

This keeps the feature reversible, easy to test, and compatible with the current architecture.

## Full Feature Potential

A complete melodic system could eventually include all of the following.

### 1. Continuity Bias

Bias note selection toward continuity with the previously played note.

Possible behaviors:
- reduce large jumps when the raw mapped pitch changes sharply
- favor stepwise motion and small intervals
- preserve phrase shape across consecutive related moves
- allow discontinuity only when the board event is musically significant

This is the highest-value general feature.

### 2. Event-Weighted Leap Permission

Not all moves should be treated equally.

The melodic stage could allow larger intervals when the move has higher energy.

Potential event signals:
- capture
- multi-capture
- move path length
- king-related checkers event
- Reversi flip count
- chess material swing
- chess tactical events if explicitly stored later

This allows the sequence to stay smooth most of the time while still producing dramatic gestures when the board action justifies them.

### 3. Interval Vocabulary Bias

Rather than using raw mapped pitches directly, the melodic stage could bias motion toward interval families.

Examples:
- prefer seconds and thirds
- occasionally allow fourths and fifths
- suppress random large leaps
- reserve large leaps for stronger moves

This would make sequences sound more phrase-like and less geometrically literal.

### 4. Register Management

Board mapping and melodic contour do not need to decide octave/register in the same way.

A future system could:
- keep scale-degree selection tied to board mapping
- separately manage register based on move energy or board region
- keep phrases centered in a usable register
- widen the register only when events become more intense

This would reduce the “all notes in one narrow lane” feeling while keeping the mapping interpretable.

### 5. Motif Persistence

The melodic stage could preserve short-term identity across related moves.

Examples:
- if the same piece moves repeatedly, preserve contour family
- if a sequence remains in a local board region, retain melodic locality
- if turn ownership changes, optionally create contrast

This would make generated phrases feel more intentional over short spans.

### 6. Game-Specific Melodic Personalities

Different games likely benefit from different melodic defaults.

Examples:
- Checkers: smoother, more groove-oriented contour
- Chess: wider interval vocabulary and stronger contrast
- Reversi: denser central movement and clustered pitch neighborhoods

This can remain behind the scenes while still using one shared melodic-bias framework.

### 7. Quantizer-Aware Melodic Decisions

A later version could account for scale mapping while choosing the melodic result.

Examples:
- choose the nearest in-scale alternative to preserve contour
- avoid repeatedly landing on weak or redundant scale positions
- prefer stable tones after stronger events

This is useful, but it adds complexity and should not be part of the first pass.

### 8. User Modes Beyond Binary Toggle

Once the feature proves useful, it could evolve from a binary toggle into explicit melodic modes.

Possible future modes:
- Direct
- Smooth
- Interval Bias
- Phrase
- Dramatic

This should only happen after the simplest form has been validated.

## What The Current Move Data Already Supports

Current `Move` data already stores enough information for a first-pass melodic stage.

Available now:
- `originIndex`
- `destinationIndex`
- `path`
- `captured`
- `isCapture`
- `isMultiCapture`
- `isKing`

This is already enough to derive useful control signals such as:
- move distance
- capture intensity
- multi-step action intensity
- rough event energy
- continuity relative to the previous note

## What Is Not Cleanly Available Yet

If a later version needs more chess-specific nuance, the current move format is not sufficient for everything.

Not explicitly stored today:
- moving piece type
- captured piece type
- promotion as a dedicated chess event
- castling as a dedicated event
- en passant as a dedicated event
- gives check
- mate / terminal tactical pressure

These can be added later if the melodic system needs richer event awareness.

## MVP-v1 Recommendation

### Feature Shape

Implement a binary toggle:

- `Melodic Bias: Off / On`

This is the correct first version because it:
- adds almost no UI clutter
- does not require rewriting the pitch architecture
- can be evaluated immediately by ear
- keeps the raw system available as a reference
- does not require changing move-history serialization

### Placement In The Pitch Pipeline

Recommended order:

1. move history
2. pitch data source selection
3. board layout / divider / bipolar mapping
4. melodic bias stage
5. quantizer / key / scale
6. root / transpose
7. output

This keeps the melodic stage above geometry, but below quantization.

### MVP-v1 Behavior

When `Melodic Bias` is enabled:
- compute the raw pitch as normal
- compare it to the previous underlying pitch
- if the pitch jump is large, pull it partway back toward the previous pitch
- reduce that pull when the move is more dramatic

Suggested event relaxers:
- capture
- multi-capture
- longer move path
- larger flip count in Reversi
- strong board-state-derived motion energy if already available

### Intended Result

The expected musical result is:
- smoother local contour
- fewer arbitrary large jumps
- preserved drama on stronger board events
- better phrase continuity without losing the connection to board behavior

## Suggested MVP-v1 Algorithm Shape

The exact coefficients can be tuned later, but the shape should be simple.

Inputs:
- previous raw pitch
- current raw pitch
- current `Move`

Derived values:
- `delta = currentRaw - previousRaw`
- `moveEnergy` from existing move metadata
- `biasStrength` reduced as move energy increases

Output behavior:
- small deltas pass through mostly unchanged
- large deltas are compressed toward the previous pitch
- energetic moves are allowed to retain more of the original jump

This should be implemented as a lightweight deterministic transform, not a search process.

## Serialization Impact

For MVP-v1, only one new serialized value is needed:
- `melodicBiasEnabled`

No move-history format change is required.

## UI Recommendation

Place the toggle in the `Pitch` section of the context menu.

Recommended label:
- `Melodic Bias`

Values:
- `Off`
- `On`

This keeps it close to the pitch interpretation pipeline and avoids making it look like a game-rule feature.

## Risks

### 1. Over-smoothing

If the bias is too strong, the module may lose the board’s identity and start sounding generic.

### 2. Quantizer Interaction

If the melodic stage pulls notes toward one another too strongly before quantization, the quantizer may over-collapse the result onto the same few scale tones.

### 3. Cross-Game Semantics

A single bias rule may sound good in Checkers but too conservative in Chess.

This is acceptable for MVP-v1, but should be revisited if the feature proves useful.

## Recommended Development Path

### Phase 1

Implement:
- `melodicBiasEnabled`
- one simple bias transform in the pitch derivation path
- no storage-format changes to `Move`

### Phase 2

Tune by ear:
- pull strength
- energy relax behavior
- per-game weighting if needed

### Phase 3

Only if the feature proves valuable:
- richer move metadata for chess
- more than one melodic mode
- quantizer-aware melodic interpretation

## Summary

The full melodic feature space is large, but Crownstep does not need that to get value immediately.

The correct first step is a binary playback-stage toggle that biases pitch continuity using the move data already stored today.

That approach is:
- implementable now
- low-risk
- reversible
- musically meaningful
- compatible with the current Crownstep architecture
