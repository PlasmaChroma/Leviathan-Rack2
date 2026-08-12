# Reliquary Embedded UnRAR Integration

**Project:** Leviathan-Rack2

**Component:** Minimal read-only RAR decoder for RSN collections

**Document status:** Candidate archive integration specification, Draft 0.2

**Date:** 2026-08-12

---

## Status and authority

This document owns Reliquary's archive-reader boundary, archive processing
algorithm, hostile-input limits, archive test corpus, and the candidate
embedded-UnRAR build design. [RELIQUARY_SPEC.md](RELIQUARY_SPEC.md) remains
authoritative for product behavior and real-time ownership.
[RELIQUARY_PHASE0_PLAN.md](RELIQUARY_PHASE0_PLAN.md) owns the feasibility work
and approval records.

Embedded UnRAR is a candidate, not an approved dependency. No source may be
vendored into the production tree and no distributable plugin may link it until
the GPL/UnRAR compatibility gate in §5.2 is resolved in writing. If that gate
fails, the project-owned `IRsnArchiveReader` boundary remains and the decoder
candidate changes.

The production Rack target is C++11. Project-owned archive interfaces and
examples in this document shall remain C++11-compatible.

---

## 1. Purpose

This document defines a self-contained, read-only RAR extraction component for Reliquary and potentially other Leviathan features.

Subject to the license and feasibility gates, the component will vendor a
pinned official portable UnRAR source, compile it into the Leviathan plugin
binary, and expose a narrow memory-oriented API. Users shall not install UnRAR,
WinRAR, a shared library, command-line executable, or any other runtime
dependency.

The primary use case is extracting `.spc` entries from `.rsn` soundtrack archives. Potential use for internally packed Leviathan assets is explicitly secondary and must be justified through measurement.

---

## 2. Decision summary

If approved, the implementation shall:

- Vendor a pinned official UnRAR source release.
- Compile UnRAR in library mode using the `RARDLL` interface.
- Link the required object code statically into the Rack plugin.
- Expose no UnRAR API outside the plugin binary.
- Extract archive entries directly to memory through the process-data callback.
- Support solid archives through sequential processing.
- Reject encryption and multi-volume workflows rather than prompt the user.
- Avoid temporary files and filesystem extraction.
- Start with upstream source largely intact, then trim only after measuring linked size.
- Maintain a small project-owned wrapper so UnRAR can be replaced without changing Reliquary.

Official UnRAR includes a library-oriented C API with archive-open,
header-read, process-file, callback, and close operations. Its upstream build
also defines a static `libunrar.a` target in `RARDLL` mode. Its source license
allows use for handling RAR archives while imposing a restriction against
recreating the compression algorithm or developing a compatible archiver. That
restriction is the reason compatibility with Leviathan's GPL-3.0-or-later
distribution must be treated as an unresolved gate rather than a notice-only
obligation.

---

## 3. Goals

1. Decode representative RSN archives reliably.
2. Support historical RAR compression used by older soundtrack collections and modern RAR formats used by repacked collections.
3. Handle solid archives correctly.
4. Produce validated SPC byte arrays without touching the filesystem.
5. Build on every Leviathan/Rack target required for release.
6. Add no runtime dynamic dependency.
7. Keep the stripped plugin-size increase within a measured, approved budget.
8. Fail safely on malformed, hostile, encrypted, oversized, or unsupported archives.
9. Preserve UnRAR licensing notices and source provenance.
10. Remain isolated behind a project-owned interface.

---

## 4. Non-goals

- Creating RAR archives.
- Repacking RSN collections.
- Implementing RAR compression.
- Password prompts or encrypted archive support.
- Multi-volume archive support.
- Recovery-volume repair.
- Extraction of files to disk.
- Restoring filesystem permissions, ownership, timestamps, ACLs, alternate streams, or links.
- Providing a general-purpose archive browser to other plugins.
- Guaranteeing that every possible RAR archive feature is accepted.
- Forking the decompression algorithm into a heavily modified private implementation before a baseline build is measured.

---

## 5. Source and license policy

### 5.1 Source provenance

The vendored tree shall originate from an official RARLAB UnRAR source release or a byte-identical, clearly traceable mirror of that release.

The dependency shall be pinned by:

- Upstream version.
- Source archive hash or commit hash.
- Import date.
- Local patch list.

### 5.2 License obligations

Leviathan declares GPL-3.0-or-later while the UnRAR source license contains a
use restriction not present in the GPL. Static inclusion in the plugin must not
be assumed compatible merely because UnRAR permits redistribution.

Before source import, the project shall record one of these outcomes:

1. A reviewed determination that the intended combined distribution can satisfy
   both licenses, including the reasoning and exact upstream version reviewed.
2. An explicit linking/distribution exception that the relevant Leviathan
   copyright holders are legally able and willing to grant, together with a
   review of all third-party GPL-covered contributions affected by that
   exception.
3. Rejection of embedded UnRAR and selection of another decoder behind
   `IRsnArchiveReader`.

This is a release gate and should receive qualified legal review where needed.
The architecture document records the engineering decision; it does not
substitute for legal advice.

Reference material for the review includes RARLAB's official
[UnRAR source page](https://www.rarlab.com/rar_add.htm), the complete license in
the exact downloaded source package, the
[RARLAB license terms](https://www.rarlab.com/license.htm), GPLv3
[sections 7 and 10](https://www.gnu.org/licenses/gpl-3.0.en.html), and the GNU
[GPL-incompatible-library guidance](https://www.gnu.org/licenses/gpl-faq.en.html#GPLIncompatibleLibs).
Web summaries do not replace the license text shipped in the pinned archive.

If UnRAR is approved, the repository and distributed plugin package shall
retain the complete UnRAR license text.

The required statement concerning prohibition of using the source to develop a RAR-compatible archiver or recreate the compression algorithm shall be included:

- In the vendored source tree.
- In third-party notices shipped with Leviathan.
- In comments accompanying any locally modified or extracted UnRAR-derived source.

Reliquary uses UnRAR only for archive handling and does not expose compression or archive-authoring functionality.

### 5.3 Update policy

UnRAR updates shall be treated as security-sensitive dependency updates.

Before updating:

- Review upstream implementation/security notes.
- Reapply the local patch series.
- Run the full archive corpus.
- Repeat binary-size measurements.
- Repeat the platform build matrix.

---

## 6. Repository layout

Recommended structure:

```text
dep/
└── unrar/
    ├── upstream/                 # pinned upstream source
    ├── patches/                  # minimal local patch series
    ├── LICENSE.txt
    ├── VERSION.txt
    └── SOURCES.md

src/reliquary/archive/
├── IRsnArchiveReader.hpp
├── UnrarRsnArchiveReader.hpp
├── UnrarRsnArchiveReader.cpp
├── UnrarMemorySink.hpp
└── UnrarError.cpp

tests/reliquary/archive/
├── RsnArchiveReaderTests.cpp
└── corpus/
```

Do not scatter UnRAR-specific headers or types through Reliquary's general module code.

---

## 7. Public project-owned API

```cpp
namespace leviathan {
namespace archive {

struct ArchiveLimits {
    uint64_t maxArchiveBytes;
    uint32_t maxEntryCount;
    uint64_t maxEntryBytes;
    uint64_t maxTotalExpandedBytes;
    uint64_t maxDictionaryBytes;
};

struct RsnEntry {
    std::string archiveName;
    std::vector<uint8_t> bytes;
    uint64_t declaredSize = 0;
    uint32_t crc32 = 0;
};

enum class RsnArchiveErrorCode {
    None,
    FileNotFound,
    OpenFailed,
    NotRar,
    DamagedArchive,
    UnsupportedVersion,
    EncryptedArchive,
    MultiVolumeArchive,
    DictionaryTooLarge,
    EntryTooLarge,
    ExpandedSizeExceeded,
    ArchiveTooLarge,
    TooManyEntries,
    ChecksumFailure,
    Cancelled,
    NoValidSpcEntries,
    InternalError
};

struct RsnArchiveError {
    RsnArchiveErrorCode code = RsnArchiveErrorCode::None;
    std::string message;
    int nativeCode = 0;
};

struct RsnArchiveResult {
    std::vector<RsnEntry> entries;
    RsnArchiveError error;
};

class IRsnArchiveReader {
public:
    virtual ~IRsnArchiveReader() = default;

    virtual RsnArchiveResult read(
        const std::string& archivePathUtf8,
        const ArchiveLimits& limits,
        const std::atomic_bool* cancelFlag = nullptr) = 0;
};

} // namespace archive
} // namespace leviathan
```

No caller outside the wrapper shall use `HANDLE`, `RAROpenArchiveDataEx`, callback messages, or any other UnRAR API type.

The wrapper converts normalized UTF-8 to the platform-native path form
internally. No production interface in this component depends on
`std::filesystem` or a C++ standard later than C++11.

---

## 8. UnRAR API usage

The wrapper shall use the library API equivalent to:

- `RAROpenArchiveEx`
- `RARReadHeaderEx`
- `RARProcessFileW` or the platform-appropriate process call
- `RARSetCallback`
- `RARCloseArchive`

The public UnRAR header defines `RAR_TEST` processing and a `UCM_PROCESSDATA` callback carrying decompressed byte buffers. The wrapper shall use this path to collect bytes directly into the current in-memory entry.

Conceptual callback:

```cpp
struct UnrarCallbackContext {
    std::vector<uint8_t>* destination = nullptr;
    const ArchiveLimits* limits = nullptr;
    const std::atomic_bool* cancelFlag = nullptr;
    uint64_t totalExpanded = 0;
    uint64_t currentEntryExpanded = 0;
    RsnArchiveError pendingError;
};

static int failCallback(UnrarCallbackContext& ctx,
                        RsnArchiveErrorCode code) {
    if (ctx.pendingError.code == RsnArchiveErrorCode::None)
        ctx.pendingError.code = code;
    return -1;
}

static int CALLBACK unrarCallback(
    UINT message,
    LPARAM userData,
    LPARAM p1,
    LPARAM p2
) {
    auto& ctx = *reinterpret_cast<UnrarCallbackContext*>(userData);

    switch (message) {
    case UCM_PROCESSDATA: {
        if (ctx.cancelFlag && ctx.cancelFlag->load(std::memory_order_relaxed))
            return failCallback(ctx, RsnArchiveErrorCode::Cancelled);

        if (p2 < 0)
            return failCallback(ctx, RsnArchiveErrorCode::DamagedArchive);

        const auto* data = reinterpret_cast<const uint8_t*>(p1);
        const size_t size = static_cast<size_t>(p2);
        const uint64_t size64 = static_cast<uint64_t>(size);

        if (size != 0 && data == nullptr)
            return failCallback(ctx, RsnArchiveErrorCode::DamagedArchive);

        if (ctx.totalExpanded > ctx.limits->maxTotalExpandedBytes ||
            size64 > ctx.limits->maxTotalExpandedBytes - ctx.totalExpanded)
            return failCallback(ctx, RsnArchiveErrorCode::ExpandedSizeExceeded);

        if (ctx.currentEntryExpanded > ctx.limits->maxEntryBytes ||
            size64 > ctx.limits->maxEntryBytes - ctx.currentEntryExpanded)
            return failCallback(ctx, RsnArchiveErrorCode::EntryTooLarge);

        ctx.totalExpanded += size64;
        ctx.currentEntryExpanded += size64;

        // Count discarded solid-history bytes too. Only retention is optional.
        if (ctx.destination != nullptr && size != 0) {
            if (ctx.destination->size() > ctx.limits->maxEntryBytes ||
                size > ctx.limits->maxEntryBytes - ctx.destination->size())
                return failCallback(ctx, RsnArchiveErrorCode::EntryTooLarge);
            try {
                ctx.destination->insert(ctx.destination->end(), data, data + size);
            }
            catch (...) {
                return failCallback(ctx, RsnArchiveErrorCode::InternalError);
            }
        }
        return 1;
    }

    case UCM_NEEDPASSWORD:
    case UCM_NEEDPASSWORDW:
        return failCallback(ctx, RsnArchiveErrorCode::EncryptedArchive);

    case UCM_CHANGEVOLUME:
    case UCM_CHANGEVOLUMEW:
        return failCallback(ctx, RsnArchiveErrorCode::MultiVolumeArchive);

    case UCM_LARGEDICT:
        return failCallback(ctx, RsnArchiveErrorCode::DictionaryTooLarge);

    default:
        return 1;
    }
}
```

The enum shall include `ExpandedSizeExceeded`, distinct from per-entry
`EntryTooLarge`. The exact callback return values and native user-break mapping
shall be verified against the pinned UnRAR version. No C++ exception may escape
through the C callback ABI. `pendingError` takes precedence over a generic
native cancellation/user-break result so cancellation, limits, encryption, and
volume failures remain distinguishable.

---

## 9. Archive processing algorithm

### 9.1 Preflight

1. Confirm the file exists and is a regular readable file.
2. Reject if compressed file size exceeds `maxArchiveBytes`.
3. Zero-initialize `RAROpenArchiveDataEx`, including all reserved fields.
4. Set extraction mode, the native path, callback, and user-data fields before
   `RAROpenArchiveEx`; alternatively use `RARSetCallback` only after a valid
   handle exists. Do not describe both sequences as one operation.
5. Open the archive and verify the pinned DLL API version with
   `RARGetDllVersion()` where the pinned release requires it.
6. Inspect archive flags.
7. Reject encrypted headers.
8. Reject any volume archive, including a first volume presented alone.
9. Install scope-bound archive-handle closure for every exit path.

### 9.2 Sequential loop

For every archive entry:

1. Read the next header.
2. Check entry count limit.
3. Compute the 64-bit declared expanded size.
4. Reject entries exceeding `maxEntryBytes`.
5. Reconstruct the dictionary request from `RARHeaderDataEx::DictSize`, which
   the inspected API reports in KiB, using checked multiplication by 1024; reject
   values above `maxDictionaryBytes` before processing. `UCM_LARGEDICT` remains
   a second upstream-limit defense, not the custom-limit mechanism.
6. Reject cumulative declared sizes exceeding `maxTotalExpandedBytes` when
   reliable, using checked subtraction rather than overflowing addition.
7. Reject encrypted entries and unsupported redirection/link entries.
8. Classify the regular entry using §9.2.1.
9. Reset `currentEntryExpanded`, `destination`, and `pendingError` for the entry.
10. For a retained candidate, reserve only after all declared-size checks.
11. Process with `RAR_TEST` when bytes or solid history are required; use
    `RAR_SKIP` only where §9.3 permits it.
12. Resolve callback error first, then require a successful native result. A
    checksum/hash failure rejects the archive by default.
13. Validate captured SPC content by signature and structural checks.
14. Retain valid SPC data; discard unrelated entry data.
15. Continue in archive order.

All `RARHeaderDataEx` instances and their reserved fields shall be zeroed before
each header read. The wrapper shall document reconstruction of the low/high
packed and unpacked sizes and test it at 32-bit boundaries.

### 9.2.1 Candidate classification

Filename extension alone is not authoritative. A candidate is a regular,
non-redirected entry whose declared size falls within the SPC validator's
documented structural envelope, or an entry named `.spc` whose declared size is
within the general per-entry cap. This permits extensionless SPC files while
avoiding allocation for obviously unrelated large assets.

Candidate bytes are still accepted only after signature and structural
validation. The SPC validator shall publish its minimum, normal, and supported
extended-size rules before archive implementation. A future streaming prefix
probe may reduce retention cost, but it must not create filename-only false
negatives.

### 9.3 Solid archives

Solid archives require sequential processing because later entries may depend on dictionary state established by earlier entries.

Therefore:

- The wrapper shall never seek directly to a selected track inside an unopened solid archive.
- Every preceding entry shall be processed as required by UnRAR.
- Non-SPC entries shall be processed with `RAR_TEST` and a null destination when
  necessary to preserve solid state; their actual expanded bytes still count
  toward the cumulative limit.
- A non-solid, noncandidate regular entry may use `RAR_SKIP` because no later
  dictionary depends on it.
- All valid SPC tracks shall normally be retained after the initial pass so later track changes do not repeat decompression.

### 9.4 Cancellation

The loader thread may pass a cancellation flag. Cancellation shall terminate extraction at the next callback or entry boundary and return a `Cancelled` error without publishing a partially prepared collection.

---

## 10. Format support

The first production target is practical RSN compatibility, not universal archive compatibility.

Required corpus categories:

- Stored RAR entries.
- RAR 2.x/3.x archives.
- RAR 4.x-era archives where represented by the upstream decoder.
- RAR5 archives.
- Current RAR generations supported by the pinned UnRAR source.
- Solid and non-solid archives.
- Unicode filenames.
- Archives containing unrelated text or image files in addition to SPC files.

Explicitly unsupported:

- Encrypted headers or entries.
- Split/multi-volume sets.
- Recovery archives requiring repair.
- External reference data.
- Archive authoring.

Support shall be determined by corpus tests rather than filename extension assumptions.

---

## 11. Build integration

### 11.1 Single-package requirement

The final plugin shall contain UnRAR code inside the platform-specific Rack plugin binary.

The installed plugin must not require:

- `unrar` executable.
- `rar` executable.
- `unrar.dll`.
- `libunrar.so`.
- Homebrew, apt, package managers, or system frameworks beyond normal Rack requirements.

### 11.2 Compilation model

Preferred order:

1. Compile required UnRAR translation units as an internal static archive or private object library.
2. Compile with `RARDLL` enabled.
3. Compile with position-independent code where required.
4. Link into the Leviathan Rack plugin.
5. Hide all dependency symbols from the plugin's external symbol table.

Upstream's library target compiles its object set in `RARDLL` mode and creates both shared and static libraries. Leviathan shall reuse the source list and definitions but build them through its own cross-platform dependency rules so the objects inherit the exact Rack target architecture and deployment settings.

### 11.3 Initial definitions

Required baseline definitions are expected to include:

```text
RARDLL
_FILE_OFFSET_BITS=64       # Unix-like targets where applicable
_LARGEFILE_SOURCE         # Unix-like targets where applicable
```

The upstream makefile enables `RAR_SMP` by default. Reliquary should initially omit `RAR_SMP`, allowing the single background loader thread to remain the only extraction thread.

Candidate size/scope definitions:

```text
RAR_NOCRYPT
NOVOLUME
```

These shall be enabled only after the full RSN corpus confirms that they preserve required behavior and produce worthwhile size or dependency reductions. Encryption and volume workflows are rejected at the wrapper level regardless.

### 11.4 Compiler policy

- Use the Rack/Leviathan-selected C++ compiler.
- Compile project-owned wrapper code and the pinned upstream dependency in a
  C++11-compatible mode unless a separately measured translation-unit override
  is approved; do not raise the plugin-wide language standard incidentally.
- Inherit target architecture, sysroot, deployment target, and CRT settings.
- Do not use `-march=native`.
- Do not require architecture-specific crypto instructions.
- Compile initially with normal project optimization.
- Evaluate size optimization flags for dependency translation units only after functional parity is established.
- Treat warnings from vendored source separately from Leviathan warnings without disabling warnings project-wide.

### 11.5 Symbol visibility

UnRAR symbols must not leak into Rack's shared process namespace.

Use platform-appropriate measures such as:

- Hidden default visibility for dependency objects.
- `--exclude-libs` on ELF linkers where appropriate.
- Export lists/version scripts on macOS or Linux if required.
- No export definition file for UnRAR in the final plugin DLL.
- Only Rack-required plugin symbols exported from the final binary.

---

## 12. Platform matrix

Required release gates:

| Platform | Architecture | Requirement |
|---|---|---|
| macOS | arm64 | Required |
| macOS | x86_64 | Required while Leviathan supports Intel Rack builds |
| Windows | x86_64 | Required |
| Linux | x86_64 | Required |
| Linux | arm64 | Required if/when included in Leviathan's supported release matrix |

Every target shall:

- Compile from the same pinned source tree.
- Open and extract the same core RSN corpus.
- Produce no additional runtime dependency.
- Pass malformed-input tests.
- Report stripped binary-size delta.

A universal/fat macOS dependency binary is unnecessary when Rack packages are built per architecture. Each target should compile UnRAR directly for its own architecture.

WSL may run corpus tests and compile focused harnesses, but it is not
authoritative for final Windows plugin linking. Windows x86_64 approval requires
the repository's intended Windows/MSYS2 Rack toolchain. A WSL full-plugin link
failure alone is not an UnRAR regression.

---

## 13. Binary-size control

### 13.1 Measurement method

For every required architecture, CI or the dependency spike shall produce:

1. Baseline stripped plugin binary without UnRAR.
2. Stripped plugin binary with the initial upstream-equivalent `RARDLL` object set.
3. Stripped plugin binary with accepted compile-time reductions.
4. Final packaged plugin size comparison.
5. Symbol/object contribution report where toolchain support exists.

### 13.2 Initial gates

Provisional targets:

- Preferred stripped binary increase: **≤ 750 KiB**.
- Review required: **750 KiB–1.5 MiB**.
- Reject or redesign: **> 1.5 MiB**, unless measured RSN compatibility demonstrates no credible smaller path.

These are project policy targets, not assumptions about the final result.

### 13.3 Trimming order

If size is excessive, optimize in this order:

1. Confirm dead-code stripping and hidden symbols work.
2. Remove `RAR_SMP` and thread-pool code where safe.
3. Enable verified no-crypt and no-volume configurations.
4. Remove CLI-only and standalone-executable translation units not required by the DLL API.
5. Apply dependency-only size optimization.
6. Evaluate LTO on the dependency.
7. Maintain a narrowly documented source manifest.

Do not remove historical decompression paths merely because they appear old; RSN collections may depend on them.

---

## 14. Memory and security limits

RAR 7 supports very large dictionary sizes, and upstream explicitly recommends rejecting or prompting for archives requesting extreme dictionaries. Reliquary shall always reject rather than prompt.

Initial limits should be conservative and configurable in code:

```cpp
ArchiveLimits defaultRsnLimits() {
    ArchiveLimits limits;
    limits.maxArchiveBytes = 256ull * 1024 * 1024;
    limits.maxEntryCount = 4096;
    limits.maxEntryBytes = 4ull * 1024 * 1024;
    limits.maxTotalExpandedBytes = 512ull * 1024 * 1024;
    limits.maxDictionaryBytes = 256ull * 1024 * 1024;
    return limits;
}
```

The exact defaults shall be tuned against real RSN libraries. SPC entries are normally tiny, so values far above expected SPC size provide compatibility while still bounding hostile input.

Additional requirements:

- Check all 32-bit/64-bit size combinations for overflow.
- Use compare-before-add/subtract checks; a requirement to “check overflow” is
  not satisfied by evaluating a potentially overflowing sum first.
- Reserve buffers only after limit checks.
- Never trust filenames as safe paths.
- Never create symlinks, hard links, directories, or special files.
- Abort on checksum failure by default.
- Abort on unsupported redirection/reference types.
- Catch allocation failures at the wrapper boundary and inside the C callback;
  no exception may cross the UnRAR callback ABI.
- Convert native errors into stable project error codes.

---

## 15. Threading

Extraction shall run on a single Reliquary loader thread.

The initial build shall not enable UnRAR's optional SMP path.

Benefits:

- Predictable CPU use.
- No hidden worker pool.
- Simpler cancellation.
- Less contention with Rack's audio engine.
- Smaller code and runtime state.
- Easier cross-platform verification.

Only one active UnRAR operation should run per loader service unless later testing proves the library and global state safe for concurrent instances.

The callback context, cancel flag, destination vector, native path buffers, and
archive handle must outlive the complete native operation. Cancellation does
not publish partial entries or a partial collection. Destruction and archive
close occur on the loader thread, never the Rack audio thread.

---

## 16. Error mapping

Map UnRAR native results and callbacks into stable project errors.

Examples:

| Native condition | Project error |
|---|---|
| `ERAR_BAD_ARCHIVE` after failed archive recognition | `NotRar` |
| `ERAR_BAD_DATA` | `DamagedArchive` |
| `ERAR_UNKNOWN_FORMAT` | `UnsupportedVersion` |
| `ERAR_EOPEN` / missing native path | `FileNotFound` or `OpenFailed`, selected from the preflight result |
| `ERAR_MISSING_PASSWORD` / password callback | `EncryptedArchive` |
| `ERAR_LARGE_DICT` / large-dictionary callback | `DictionaryTooLarge` |
| Volume callback | `MultiVolumeArchive` |
| Generic native user-break after callback returned `-1` | The already recorded `pendingError` |
| Native user-break without `pendingError` | `InternalError` |
| CRC/BLAKE2 verification failure reported by `RAR_TEST` | `ChecksumFailure` |

Mapping is evaluated in this order: wrapper preflight error, callback
`pendingError`, specific native result, then fallback `InternalError`. A single
native value shall not nondeterministically map to different project errors.

The user-facing message should state what action is possible, for example:

- “This RSN is encrypted; encrypted archives are not supported.”
- “This RSN is part of a multi-volume archive.”
- “This archive requests more decompression memory than Reliquary permits.”

---

## 17. Test corpus

The repository shall include redistributable synthetic or purpose-built fixtures rather than copyrighted soundtrack collections.

Required fixtures:

```text
rar3-stored.rsn
rar3-solid.rsn
rar5-stored.rsn
rar5-solid.rsn
rar-current-solid.rsn
unicode-names.rsn
mixed-entries.rsn
encrypted-entry.rsn
encrypted-headers.rsn
multivolume-part1.rar
large-dictionary.rsn
bad-header.rsn
truncated-data.rsn
bad-crc.rsn
empty.rsn
not-rar.rsn
```

Each valid RSN fixture shall contain small synthetic SPC-shaped test files with known hashes.

### 17.1 Unit tests

- Open/close behavior.
- Entry iteration.
- 64-bit size reconstruction.
- Callback accumulation.
- Cancellation.
- Error conversion.
- SPC filtering.
- Limits and overflow protection.
- Actual-expanded accounting for discarded solid-history entries.
- Dictionary KiB-to-byte conversion and the 256 MiB custom cap.
- Distinct callback errors for cancellation, entry size, cumulative size,
  encryption, volume, and allocation failure.
- Zero-initialized reserved API fields and 64-bit low/high reconstruction.
- Extensionless valid SPC and misleading `.spc` non-SPC entries.
- Non-solid `RAR_SKIP` versus solid `RAR_TEST` selection.

### 17.2 Cross-platform smoke tests

For each required target:

- Extract valid solid and non-solid fixtures.
- Compare entry names, counts, sizes, and hashes.
- Confirm encrypted and multi-volume fixtures are rejected.
- Confirm malformed fixtures do not crash or hang.
- Inspect final dynamic dependencies.
- Record binary-size delta.

### 17.3 Fuzzing

The wrapper boundary and selected archive-open/read/process paths should receive fuzz testing where practical.

At minimum:

- Mutate headers.
- Truncate archives at random offsets.
- Corrupt declared sizes.
- Corrupt dictionary fields.
- Corrupt entry CRCs.
- Exercise repeated load/cancel/destroy cycles.
- Assert wall-clock and allocation ceilings in the harness so a hang or
  decompression bomb fails the test rather than the machine running it.

RAR fixtures shall be generated only with an authorized archive-writing tool.
The repository may store redistributable binary fixtures and their provenance;
reproducing the corpus must not require Reliquary or UnRAR to author archives.
Each fixture record shall name its authoring tool/version, options, expected
entry hashes, license/provenance, and whether byte-for-byte regeneration is
expected.

---

## 18. Internal asset-pack evaluation

The presence of an embedded decoder permits, but does not automatically justify, RAR-packed Leviathan resources.

### 18.1 Rules

- Internal asset RAR creation must use an authorized external RAR authoring tool in the build pipeline; UnRAR shall never be extended to create archives.
- Canonical source assets remain unpacked in the repository.
- Packed resources are generated release artifacts.
- Runtime asset extraction must remain outside the audio thread.
- Asset archives shall use deterministic ordering and documented authoring settings where possible.

### 18.2 Required measurement

Before adoption, compare:

- Loose installed asset size.
- RAR installed asset size.
- Final distributable package size with each approach.
- Build complexity.
- First-load latency.
- Peak memory.
- Source-control and debugging cost.

Likely candidates are repetitive raw assets, wavetables, JSON/text databases, or large families of similar uncompressed files. Already compressed PNG, WebP, compressed audio, and fonts may gain little.

Internal asset packing is not part of the initial Reliquary milestone.

---

## 19. Implementation phases

### Phase U-1 — legal and provenance gate

- Select the exact official source release proposed for evaluation.
- Record its source hash and complete license text without importing it into the
  production dependency tree.
- Resolve §5.2 for Leviathan's GPL-3.0-or-later distribution.
- Stop or select another decoder if compatibility is not approved.

### Phase U0 — baseline source import

- Begin only after Phase U-1 passes.
- Pin official source.
- Add license and provenance.
- Build upstream-equivalent static library/object target.
- Link into a minimal test executable.
- Open one known RSN.

### Phase U1 — memory extraction wrapper

- Implement project-owned API.
- Capture `UCM_PROCESSDATA` into bounded vectors.
- Enumerate and validate entries.
- Support cancellation.
- Add error mapping.

### Phase U2 — solid/archive corpus

- Build synthetic RAR-generation corpus.
- Verify solid sequential behavior.
- Verify Unicode names.
- Verify malformed and unsupported cases.

### Phase U3 — Rack integration

- Compile with Rack toolchains.
- Link privately into Leviathan.
- Hide symbols.
- Verify no dynamic dependencies.
- Run required platform matrix.

### Phase U4 — size optimization

- Measure baseline delta.
- Remove SMP.
- Evaluate no-crypt/no-volume defines.
- Remove demonstrably unused CLI objects.
- Apply size optimization/LTO only where stable.
- Publish final source manifest and size report.

### Phase U5 — optional asset study

- Evaluate selected asset families.
- Compare package and installed sizes.
- Adopt only with meaningful measured benefit.

---

## 20. Acceptance criteria

The embedded UnRAR component is approved for Reliquary when:

1. The GPL/UnRAR combined-distribution gate is approved in writing for the exact pinned release.
2. It extracts valid SPC entries from representative solid and non-solid RSN archives.
3. It supports the historical and current RAR generations represented in the approved corpus.
4. It builds for macOS arm64, macOS x86_64, Windows x86_64, and Linux x86_64.
5. It adds no runtime library or executable dependency.
6. The final plugin exports no public UnRAR symbols beyond unavoidable platform behavior.
7. It writes no temporary files.
8. It rejects encrypted, multi-volume, oversized, excessive-dictionary, malformed, and checksum-failing input safely.
9. It counts actual discarded solid-history output against cumulative limits and honors cancellation.
10. Its stripped binary-size delta falls within the approved project budget.
11. License, acknowledgements, patch provenance, and source notices are complete.
12. The wrapper exposes no UnRAR-specific types to Reliquary and compiles under the production C++11 policy.
13. The same corpus produces equivalent entry hashes on every supported architecture.

---

## 21. Rejection and fallback criteria

Reconsider UnRAR if any of the following remain after reasonable integration work:

- A required architecture cannot build without invasive platform patches.
- The stripped binary increase exceeds 1.5 MiB and cannot be reduced safely.
- The DLL callback path cannot reliably capture solid entries to memory.
- The implementation introduces unstable global state inside Rack.
- License obligations conflict with Leviathan's intended distribution.
- Security maintenance becomes impractical.

Fallback candidates may include a narrowly configured statically linked archive library or an alternative decoder behind `IRsnArchiveReader`. Reliquary itself shall not change because of this substitution.

---

## 22. Architectural summary

```text
.rsn file
   ↓
UnrarRsnArchiveReader
   ↓  RAROpenArchiveEx / RARReadHeaderEx
Official UnRAR decoder
   ↓  UCM_PROCESSDATA callback
Bounded in-memory entry sink
   ↓
SPC signature and structure validator
   ↓
vector<RsnEntry>
   ↓
Reliquary SpcCollection loader
```

The component's governing principle is containment: retain upstream decompression expertise, expose only the tiny read-only behavior Reliquary needs, and make portability, size, and hostile-input resistance measurable release gates.
