# Chimera quick guide

Chimera is a one-Reel-per-module instrument. Add another Chimera to play a second Reel at the same time. The audio engine stores its Reel at 48 kHz and Rack embeds it when the patch is saved. A source WAV does not need to remain at its original path after a successful patch save.

## Panel

The top display shows a cached waveform with amber Splice markers and a live playhead. Mono audio uses one full-height trace; when the left and right samples differ, it shows separate upper (teal) and lower (violet) channel traces with the same amplitude scale. The red-tinted span marks audio written since the last waveform refresh. A recording or edit can be audible before the display cache catches up. The bottom display row shows the recording state on the left, the Reel duration and selected Splice (`SPL`) in the middle, and **SAVED** or **UNSAVED** on the right; `>` marks a pending selection. An error is written as text; open the module menu for its specific message.

Click and release **REC** once to start the default recording action; click and release again to stop or cancel an arm. With CLOCK connected, an idle REC request arms until the next accepted edge. The display distinguishes **ARM CURRENT**, **ARM APPEND**, **ARM STOP**, **REC CURRENT**, and **REC APPEND**. Use the module menu's explicit **Record Current**, **Record Append**, and **Stop recording / cancel arm** commands if the default assignment is not the one you want. The menu's **REC assignment** setting chooses which destination a normal REC click uses. During Record Current, writing starts where the primary playhead is and follows each committed Splice selection from its playhead address; a pending selection does not change the destination yet. **SPLICE** adds a marker; **SHIFT** requests the next Splice; **Organize** selects a Splice directly.

The module menu contains the input-gain choices, Play/Clock/Vari-Speed modes, behavior switches, chord ratios, and options-text import/export. **Load Reel WAV** imports one Reel into this module and asks before replacing existing audio. **Export Reel WAV** writes the current Reel to a selected file and asks before overwriting an existing file. Rack patch Save writes a separate embedded asset; use it to preserve the Reel with the patch. The bottom jacks expose stereo input/output, CV/EOSG, and the control CV/gates.

## Editing and recovery

Select a Splice with Organize or SHIFT, then use the named marker, Erase, Delete, or Clear actions in the menu. Audio edits are committed changes and ask for confirmation; they cannot be undone. **Undo marker edit** and **Redo marker edit** swap one small in-memory marker table without copying audio. Moving or removing a marker preserves playback, and changed splice boundaries take effect at the natural playback handoff. Intervening recording or marker changes invalidate marker Undo. Ordinary knob drags use Rack's normal history.

Chimera captures a pre-record checkpoint without delaying REC and, after recording stops, requests a completed cut. A Rack save, waveform refresh, or earlier checkpoint can keep its frozen audio while a new take captures a separate pre-record version. Only overwritten audio pages need copies; background work replenishes a bounded scratch pool without allocating on the audio thread.

Two additional pre-record cuts can overlap the ordinary save/display/recovery snapshot. If both extra slots remain occupied when another take starts, recording proceeds and the display shows **NO PRE-REC CUT** until the next take or Reel replacement. **PRE-REC SAVING** means a cut is being captured or saved; it is not crash-recoverable until the save completes. **PRE-REC FAILED** means it could not be completed. The context menu reports the latest take's pre-record status, including successful completion. An older checkpoint finishing later never marks a newer take as protected or replaces a newer cut from this module in the recovery journal.

If storage remains stalled or scratch allocation cannot keep up and all available pages are consumed, recording stops before it can damage protected audio. Once the held versions finish and their pages are recycled, a fresh REC can start again. This is an exceptional resource limit, rather than a normal consequence of recording during a save.

During a long recording Chimera requests a cut no more often than every ten seconds when the snapshot path is free. The dated **Restore pre-recording checkpoint** and **Recover latest checkpoint** menu actions replace this module's Reel. They restore the last *completed* checkpoint, which may belong to an earlier take; samples written after that cut may be lost. A missed pre-record cut is never replaced by a later cut labeled as pre-record. Save the Rack patch after restoring if the restored version is the one you want to keep in the patch archive.

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


### EOSG feedback to SHIFT

New modules enable **EOSG: primary boundaries only** in the module menu. EOSG
then pulses at the primary Gene boundary (or primary whole-Splice traversal),
so overlapping secondary voices cannot queue extra SHIFT advances. Disable it
to restore pulses from all voices. The choice is saved in the Rack patch;
older patches retain all-voice behavior until you enable the new option.

For whole-Splice auto-advancement, use whole-Splice Gene Size (fully
counterclockwise) and enable **Immediate transitions**. With immediate
transitions disabled, the returning EOSG pulse arrives after its originating
boundary and the requested selection waits for the next primary boundary.
Finite Gene Size advances on Gene completions, not necessarily at the end of
the Splice. CLOCK and Organize modulation can still affect selection timing.
The EOSG option is Chimera-specific and is not part of Morphagene options-text.
