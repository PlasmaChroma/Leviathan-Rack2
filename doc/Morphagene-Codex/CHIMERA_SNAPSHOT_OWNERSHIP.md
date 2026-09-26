# Chimera overlapping snapshot ownership

The ordinary save/display/edit/recovery path retains Reel slot 0 and its existing command/acknowledgment lifecycle. REC uses that path when available. During contention it can capture slots 1 and 2 independently, without waiting for a save or stealing a reader's immutable cut. The snapshot index is carried explicitly through recovery, bundle encoding, and WAV writing. Existing callers default to slot 0; patch formats are unchanged.

## Core and worker responsibilities

- The core owns all active page mappings, physical-page reference counts, snapshot state, free-page indices, and capture/reclaim cursors. A snapshot increments a page reference before its first overwrite, or during incremental capture. Only a page with more than one reference needs a copy. Unchanged pages can be shared by all three cuts.
- Capture and reclaim share a total budget of eight page operations per core tick, round-robin across slots. Scratch adoption also handles at most eight page indices per tick. Beginning a cut captures bounded metadata, not a full audio buffer or page-table copy.
- A worker reads a slot only after acquire publication of readiness. Its source Reel stays registered until all slots retire. Release commands name the take generation and slot; stale releases cannot free a later take. Module teardown transfers the shared source lifetime to every in-flight save, edit, recovery, and overlapping-cut ticket.
- Overlapping pre-record jobs publish serially, after any older ordinary pre-record job. The per-take status still uses generation-checked completion. A busy job queue retains the exact cut for retry; it never recaptures overwritten samples later.

## Scratch allocation and budget

Production registry admission prepares two extra snapshot tables, fixed-capacity freelist/reference storage, and an initial scratch chunk of up to 1,024 stereo pages (2 MiB). This supplements the existing active/reserve pool; it does not allocate another complete Reel. Small Reels cap scratch at twice their logical page count.

The cap for a full Reel is 8,192 scratch pages (16 MiB). A shared, independent allocation worker polls demand every 5 ms, allocates and zero-touches fixed-address chunks off audio, then release-publishes their availability. Audio never grows a container, allocates memory, destroys a chunk, locks the service, or wakes its worker. The service has weak registrations; transient strong references protect allocations during replenishment. It shuts down explicitly with the Chimera services.

Growth is requested below 1,024 free pages, after any previously published pages have been adopted. Chunks remain reusable until the Reel dies; no background operation relocates an existing page. The store ledger reserves the entire possible scratch growth and mapping/reference bookkeeping up front. The two-resident-store limit is 352 MiB; a full prepared Reel charges 176,060,801 bytes, although only the initial scratch chunk is actually allocated at admission. The ledger is per module, not a global system-memory limit.

At exhaustion, the write fails before changing either active or frozen audio. Existing snapshots remain valid. The stopped take never silently resumes; a fresh REC may clear the exhaustion latch after every held snapshot drains and pages are available. If all snapshot slots are occupied, the existing explicit no-pre-record warning applies. Neither bounded memory nor a finite slot count promises unlimited rapid takes during an indefinitely stalled disk operation.

## Validation

`test-chimera-overlap` exercises three versions, shared pages, out-of-order reclamation, slot reuse, incremental work bounds, growth, allocation failure, exhaustion, restart, and concurrent readers. It is included in `test-chimera-phase2`.

The module suite blocks the real Rack save encoder, records two takes on another thread, checks audio allocation/deallocation traps and the original saved version, then verifies the latest durable pre-record cut. It also exercises source retention during teardown of an overlapping worker and stopped-engine draining. WSL TSAN and ASan/UBSan runs cover the standalone overlap fixture; TSAN also covers the existing Reel concurrency fixture. Native Rack-linked tests and the Windows plugin link remain authoritative for Windows behavior.
