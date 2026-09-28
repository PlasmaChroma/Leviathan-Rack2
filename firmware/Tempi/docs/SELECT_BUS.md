# Select Bus: recovered receive protocol

**Parser:** program `0x71AA–0x7334`  
**Header bytes:** `00 02 2D`  
**Direction observed:** receive only.

This is a reconstruction of the parser in the supplied firmware, not a universal specification for Make Noise devices or all Select Bus revisions. The header is identified from initialized RAM and comparison instructions; no unsupported assignment of a manufacturer-ID registry entry is necessary.

## Receive path

The Timer2 interrupt polls UART1's receive flag and deposits a byte at RAM `0xA60 + write_index`. The ring has **460 bytes**, ending at `0xC2B`, and wraps at `0x1CC`. The foreground parser consumes at most one queued byte per call. Producer index is at `0x44E`; consumer index at `0x595`.

No separate UART receive interrupt is required by this observed path. A completely occupied ring can become ambiguous without an explicit full flag; no overflow protection claim is made. On UART overrun the parser toggles continuous receive off and back on. Explicit framing-error handling was not identified.

## Status handling

| Byte | Interpretation in this parser |
|---|---|
| `C0` | Begin one-data-byte program/state request |
| `F0` | Begin a header-checked proprietary payload |
| `F4` | Begin one-data-byte copy/store request |
| `F7` | Clear parser status and payload index |
| Other bytes `80..FF` | Ignored without generally cancelling the current accepted status |

Only the exact `C0` status is selected; `C1..CF` are not equivalent alternatives. A consumed `C0` data byte ends that message, so generic MIDI running-status behavior must not be assumed.

## Program/state request

```text
C0 ss
```

The data byte is copied to RAM `0x5FA`, and parser status is cleared. The higher-level state-selection service applies range, follow, bank/offset, and local-control logic. A parser accepting a byte is not proof that the physical module will immediately recall that number in every mode.

A test frame `C0 25` queues decimal state 37. A later `C1 26` does not replace it. `C0 F8 16` queues decimal 22: the interleaved `F8` is ignored by this parser rather than interpreted here as a scheduling clock.

## Copy/store dispatch

```text
F4 dd
```

For `dd < 64`, the parser calls `0x88D4` with W=`dd` and RAM `0x029`=0. This is the copy-current-program-to-target-state path. For `64 <= dd < 128`, it calls the same routine with W=0 and RAM `0x029`=1, selecting the persistent-store path. It also clears the Mesh bitmap and performs follow-dependent UI bookkeeping.

The included tests verify these exact dispatcher calls with a hook. They do not execute a real device's EEPROM save or establish its duration.

**Persistent commands can overwrite saved data on hardware.** The tools in this package are offline and never send any bytes to a serial port or module.

## Header-checked payload

```text
F0 00 02 2D [commands...] F7
```

A mismatch in any of the first three data bytes clears status/index. After a correct header, the parser accepts the following subcommands:

| Command | Following data | Recovered action |
|---:|---|---|
| `00` | State `ss` | Clear that state bit in the Mesh bitmap |
| `01` | State `ss` | Set that state bit in the Mesh bitmap |
| `02` | None | Call `0x8EE6`: initialize the current cached state |
| `03` | None | Call `0x964A`: revert/reload dirty states and settings |

The Mesh bitmap is eight bytes at RAM `0x544`. Its helper at `0x7D08` also supports tests, clearing all bits, and checking whether any bits are active. Follow state gates the helper's query behavior, so bitmap mutation and its effect on selection are not the same operation.

Commands 0 and 1 consume a command/state pair, then rewind the payload index to accept another pair. Commands 2 and 3 invoke their action after the command byte and return toward command parsing. The byte-level trace tests exercise the header, set/clear pairs, invalid header handling, and message termination; not every combination involving initialization/revert is exhaustively tested.

Example, setting Mesh states 7 and 63 and then clearing state 7:

```text
F0 00 02 2D 01 07 01 3F 00 07 F7
```

The tested final bitmap has only state 63 set.

## Electrical and timing limits

UART receive is configured as asynchronous 8-bit input, with TX disabled. The low-speed, 8-bit baud generator has SPBRG1=32. Therefore its nominal receiver baud is `Fosc/2112`. Actual `Fosc` has not been established.

Do not assume that this stream belongs on a conventional MIDI DIN port, that UART logic levels equal Eurorack CV levels, or that arbitrary power-bus wiring is safe. The official manual's receive-only Select Bus description corroborates the interface role; it does not replace an electrical schematic.
