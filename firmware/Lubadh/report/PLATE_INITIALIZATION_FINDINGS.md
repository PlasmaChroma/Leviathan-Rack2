# Executed MonoPlate initialization and state map

`probes/probe_plate_initialization.py` executes the inlined plate-constructor
slice at `0x3cff0..0x3d618` on original ARM bytes, stopping before the next
unrelated string/asset construction. It starts with Channel in r4 and zeroed
fixture memory. The plate starts at **Channel+25184**; 146,688 bytes are inspected.
The slice executes 376 distinct instruction addresses, including the constructor
branch at `0x3da44`. Only the existing memory-service imports are hooked.

All thirteen descriptors identified by process loads point to in-object
sample storage. Their capacities and processing spans match, write indices
start at zero, and every sample in the thirteen buffers is zero after this
constructor slice. This is an executed initialization result, **not a recovered
or verified complete plate recurrence**.

Continuation: `PLATE_PROCESS_FINDINGS.md` now supplies the independently checked
complete recurrence, tables and reverb publication; `OUTPUT_COLOR_PIPELINE.md`
checks the connected output effects. The static leads below retain their
historical scope and are superseded by those executed results.

| Pointer field relative to plate | Descriptor offset | Sample storage offset | Capacity / initial processing span |
|---|---:|---:|---:|
| 0xf58 | 3920 | 0 | 980 |
| 0x1330 | 4904 | 3972 | 233 |
| 0x1604 | 5628 | 4928 | 175 |
| 0x1fd8 | 8144 | 5652 | 623 |
| 0x270c | 9988 | 8168 | 455 |
| 0x3054 | 12364 | 10012 | 588 |
| 0x9ed4 | 40652 | 12920 | 6933 |
| 0xcd34 | 52524 | 40688 | 2959 |
| 0x11ddc | 73172 | 52548 | 5156 |
| 0x12410 | 74760 | 73192 | 392 |
| 0x198a4 | 104604 | 75316 | 7322 |
| 0x1dd28 | 122144 | 104640 | 4376 |
| 0x23cd0 | 146632 | 122168 | 6116 |

For each pointer field, +4 stores the current write index and +8 the processing
span. The descriptor itself stores the sample pointer and capacity. These are
separate objects and must not be collapsed into a vector header interpretation.

## Static process leads supported by this map

The following observations are process-listing recovery; they have not yet been
checked by an independent sample model:

- `0x498cc..0x49928` reads and then writes the 980-sample input delay. Input
  filtering occurs before that delay write, with branches selected by the
  state at plate+0xf74.
- `0x4992c..0x49a38` operates the 233/175/623/455 delay stages in sequence.
  Each reads an old delayed value, writes an input-plus-coefficient-times-delay
  value, then forms an output by subtracting coefficient times the new write.
  Thus these have actual feedback allpass algebra, unlike merely summing delayed
  copies. Coefficients and signs still require independent reconstruction.
- Output tap reads are gathered before this sample's tank writes. Checked
  descriptors anchor seven static read offsets: 437 and 4889 on the 6933 buffer,
  2655 on 2959, 3282 on 5156, 3272 on 7322, 307 on 4376, and 1753 on 6116.
  The listing multiplies them by 1, 0.125, 0.3, 0.2, 0.25, 0.8753 and 0.5,
  respectively. At `0x49f08..0x49f3c`, their combination is
  `tap4889 + tap437 - tap2655 + tap3282 - tap3272 - tap307 - tap1753`.
  This is a static output lead, not measured frequency or decay behavior.
- The 588/392 stages use phase-table-driven fractional cubic reads. The process
  invokes `floorf` at `0x49bb0` and `0x49d70`; the original-byte harness currently
  permits memory services only. A future explicit, provenance-checked math hook
  can supply floor for finite phases while leaving the delay/filter DSP original.
  Do not silently bypass these modulation stages to make a probe pass.
- `0x49f50..0x49fa0` mixes the wet tap sum with the input-filtered sample using fields
  at plate+146668 and plate+146664, then applies a rational saturation and clamp.
  Even a dry configuration can therefore still pass through the plate's final
  nonlinear output stage. This needs a numerical probe before becoming normative.

## Next work and limits

Use the initialized object as the basis for full original-byte impulse/silence
execution. Recover each filter mode, feedback/allpass coefficient, modulation
table/rate and wet/dry/control setter first; compare an independently expressed
complete recurrence and internal state trajectories. Cover delayed onset, ring
wrap, repeated impulse, amount changes and reset. Only then connect the plate
to the persistent callback and call its sound contract closed.

Constructor words, descriptor metadata, ELF hash and coverage are in
`probes/plate_initialization_probe_results.json`. Nonzero words include copied
tables and absolute fixture pointers; that dump is research state, not a portable
native layout to transplant. No audible equivalence, hardware timing or complete
plate impulse-response agreement is claimed.

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_plate_initialization.py
```
