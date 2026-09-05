# Phonex: SDT Pronunciation Support

Status: proposed. This is a focused extension to the existing Phonex design in
[phonex.md](phonex.md).

## Goal

Make pronounced SDT words and phrases easy to enter, inspect, correct, and reuse
in Phonex's 64-slot user bank. Preserve the user's spelling while allowing an
explicit pronunciation. SDT-SL, language generation, dialect synthesis, and a
larger panel are outside this phase.

## Current behavior

Phonex resolves ordinary text through an authored dictionary and basic English
letter-to-sound rules. Unknown words containing an apostrophe or exceeding 32
characters fall back to letter spelling. Hyphen-separated pieces become separate
words with silence between them; they are not true syllable groups. Non-ASCII
characters are skipped with a warning.

Explicit `[PHONEMES]` already bypass pronunciation guessing and can be mixed with
ordinary text. Phonemes support vowel stress and explicit `SIL` pauses, but no
authored duration. The user bank saves source text and recompiles it on load.

## Pronunciation resolution

- Expand the authored pronunciation table with auditioned SDT words. Treat these
  as spelling-to-sound entries, not definitions or translations.
- Expose each word's resolved phonemes and their origin: authored, guessed,
  spelled out, or explicit.
- Support documented SDT accented forms through explicit Unicode normalization
  and pronunciation rules. Preserve the original text. Unsupported forms must
  produce a specific diagnostic rather than silently lose sounds.
- Keep existing English fallback available. Do not globally reinterpret English
  vowel combinations to accommodate SDT.
- Represent syllable groups independently of silence. A group boundary organizes
  editing without inserting a pause; `SIL` remains an intentional pause.
- Retain the current phone inventory and stress notation for the first version.
  Approximations must be identified; new consonant models and vowel-hold controls
  are later work.

## Compact pronunciation editor

Open a pronunciation view from the existing user-word editor or its context menu.
Use a popup or overlay; no additional module width is required.

The view contains:

1. Original word or phrase text.
2. A word-by-word phoneme breakdown with resolution origins.
3. Editable phonemes grouped into syllables, with explicit stress and pauses.
4. A small sound guide with familiar examples and clickable sound audition.
5. Audition, Apply, and Reset to Automatic actions.

For example, a user seeking approximately “loh-rah-nah” could audition
`L OW R AA N AA`, grouped as `L OW / R AA / N AA`. The slashes here illustrate
editor grouping, not a change to existing bracket syntax or a canonical SDT
pronunciation.

Edits remain drafts until Apply. Audition plays the draft through Phonex's current
voice settings without replacing the saved entry. Apply validates the complete
candidate and publishes it only on success. Errors identify the offending sound
or text span and preserve the last valid entry. Bind drafts to their original
slot so incoming Word CV cannot redirect an edit to another slot.

## Storage and existing banks

Retain the existing `userBank` source-text array and add optional, versioned
per-slot pronunciation metadata. Store an override as an explicit phoneme script
for the complete entry, plus optional editor grouping. Keep display spelling
separate from the override. Overrides are patch-local in this phase; the shipped
dictionary provides shared pronunciations.

Resolution order is an explicit entry override, then the existing text compiler
with authored dictionary entries and fallback rules. Bracketed phonemes inside
source text retain their current explicit behavior.

Existing banks load without manual re-entry. After installing an updated plugin
and reopening a patch, plain-text entries benefit from dictionary improvements
because they are recompiled. This may intentionally change their pronunciation.
Explicit overrides and bracketed phonemes bypass those dictionary changes.
Opening the new editor alone must not change an entry's sound.

Extend the existing word-bank semantic interface to inspect, validate, apply,
and clear pronunciation overrides with the same revision checks as text edits.
Changing source text must invalidate an old override explicitly; legacy text-only
replacement operations clear it. Keep parameter, input, and output IDs stable.

## Implementation and validation

Expose a shared pronunciation result from `PhonexPronunciation` containing the
phone script, source spans, resolution origins, and diagnostics. Both the editor
and compiler must consume this result so their interpretations agree. Extend the
existing sequence compiler and mailbox publication path as needed.

All parsing, dictionary lookup, draft preparation, and compilation stay outside
audio processing. Playback uses bounded, precompiled data. Preserve existing
text and sequence capacity checks.

Validate dictionary precedence, unknown-word fallback, supported accented forms,
explicit phonemes, grouping without added silence, override reset, legacy JSON
loading, metadata round trips, revision conflicts, and invalid-edit preservation.
Audition a small agreed set of SDT phrases and ordinary English words to check
pronunciation and regressions. Verify the native Windows plugin build.
