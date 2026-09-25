# Chimera quick guide

Chimera is a one-Reel-per-module instrument. Add another Chimera to play a second Reel at the same time. The audio engine stores its Reel at 48 kHz and Rack embeds it when the patch is saved. A source WAV does not need to remain at its original path after a successful patch save.

## Panel

The top display shows a cached waveform with amber Splice markers and a live playhead. The red-tinted span marks audio written since the last waveform refresh. A recording or edit can be audible before the display cache catches up. The display names the recording state, selected Splice (`SPL`), pending selection (`>`), and whether the current Reel is **SAVED** or **UNSAVED**. An error is written as text; open the module menu for its specific message.

Click and release **REC** once to start the default recording action; click and release again to stop or cancel an arm. With CLOCK connected, an idle REC request arms until the next accepted edge. The display distinguishes **ARM CURRENT**, **ARM APPEND**, **ARM STOP**, **REC CURRENT**, and **REC APPEND**. Use the module menu's explicit **Record Current**, **Record Append**, and **Stop recording / cancel arm** commands if the default assignment is not the one you want. The menu's **REC assignment** setting chooses which destination a normal REC click uses. **SPLICE** adds a marker; **SHIFT** requests the next Splice; **Organize** selects a Splice directly.

The module menu contains the input-gain choices, Play/Clock/Vari-Speed modes, behavior switches, chord ratios, and options-text import/export. **Load Reel WAV** imports one Reel into this module and asks before replacing existing audio. **Export Reel WAV** writes the current Reel to a selected file and asks before overwriting an existing file. Rack patch Save writes a separate embedded asset; use it to preserve the Reel with the patch. The bottom jacks expose stereo input/output, CV/EOSG, and the control CV/gates.

## Editing and recovery

Select a Splice with Organize or SHIFT, then use the named marker, Erase, Delete, or Clear actions in the menu. Destructive actions ask for confirmation. Module-local **Undo Reel edit** and **Redo Reel edit** restore one edit step; ordinary knob drags use Rack's normal history. Moving or removing a marker preserves playback and stores its Undo in memory. Changed splice boundaries take effect at the natural playback handoff. Audio edits use a file-backed checkpoint. A new edit replaces the previous edit history, and intervening recording or marker changes invalidate marker Undo.

Chimera checkpoints a pre-record cut and, after recording stops, a completed cut. During a long recording it requests a cut no more often than every ten seconds when the snapshot path is free. The dated **Restore pre-recording checkpoint** and **Recover latest checkpoint** menu actions replace this module's Reel. They restore the last *completed* checkpoint; samples written after that cut may be lost. Save the Rack patch after restoring if the restored version is the one you want to keep in the patch archive.

## What the display means

**UNSAVED** means the active Reel audio or markers differ from the last embedded save, or no embedded Reel exists yet. **SAVED** means the current Reel revisions match the embedded save. **/ IO** means a Reel I/O job is active. **SAVE ERROR**, **RATE ERROR**, **REEL NOT READY**, and **REEL ERROR** require attention; the module menu provides the available diagnostic text. A flashing or dim light alone does not establish whether audio is durable.


## Playback quality and background work

**Bandlimited playback (higher CPU)** in the module menu reduces aliasing when
speeding up a Reel or using pitched Genes. It is saved with the patch and fades
in when switched. Leave it off for the original cubic sound and lower CPU use.
It suppresses the reviewed 15 kHz-at-2x alias by about 59 dB; sharp PM and other
extreme modulation can still alias. In the dense-grain recording benchmark it
used about 5.9% of one core versus 1.8% with the setting off, excluding host
sample-rate conversion and Rack/UI overhead.

Full Reels now use about 151 MiB of accounted audio/filter payload, including
recording snapshot reserve. A prepared replacement can temporarily bring that
to about 302 MiB; the payload limit is 304 MiB per module. This storage supports
instant quality switching and coherent filtering during recording.

Reel jobs and recovery now continue when the host stops stepping the module
widget. New edit checkpoints use exclusive session leases. Unlocked abandoned
sessions older than 24 hours are cleaned in bounded background passes; live
Undo files, unknown files and legacy checkpoint directories are retained.
