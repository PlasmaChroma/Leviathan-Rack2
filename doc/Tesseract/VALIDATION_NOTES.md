# Validation performed for this handoff

Date: 2026-09-28.

## Completed

The original archive's `validate_and_extract.py` was rerun successfully. Its captured report is `provenance/original_harness_rerun.log`. This is shared offline PIC-harness validation, not physical-device validation.

The handoff checker passed: 1,743 ratio fixture rows, 42 phase rows, 249 alignment rows, 64 PRNG rows, 64 factory States, 64 EEPROM-addressing cases, ten bundled Leading examples, fourteen bundled tempo-control examples, ADC range/sentinel checks, both 64-State default JSON layers, and the explicitly chosen policy examples. It also checked the main specification's factory table, JSON examples, and packaging.

The main Markdown parsed successfully with 19 tables and 54 fenced code blocks. All three fenced JSON examples parsed. Its 26 navigation links resolve to document headings. Its 132 acceptance-test IDs are unique. These are required future implementation tests, not a claim that 132 tests of a Rack module have been executed.

The C++ arithmetic snippets extracted from the specification were compiled with g++ using `-std=c++17 -O2 -Wall -Wextra -Werror -fsanitize=undefined -fno-sanitize-recover=all`. The executable matched all 1,743 ratio, 42 phase, and 64 PRNG fixture rows without an UndefinedBehaviorSanitizer failure. The source of this narrow smoke check is `tools/spec_kernel_smoke.cpp`.

## Not completed or not applicable

No Rack module was implemented or built during this specification task. No Rack panel was visually tested. No physical TEMPI was measured. The complete original timing harness was not freshly rerun to completion; `fixtures/timing_validation.json` is the bundled earlier report. The handoff's policy examples are software decisions, not newly recovered hardware facts.

## Optional C++ snippet check

From this folder, with a suitable g++ toolchain:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Werror -fsanitize=undefined -fno-sanitize-recover=all tools/spec_kernel_smoke.cpp -o /tmp/tempi_spec_kernel_smoke
/tmp/tempi_spec_kernel_smoke fixtures
```

This only tests the extracted arithmetic snippets. The future project's tests must call the actual production C++ core and must not substitute this standalone snippet test for that requirement.

## Implementation-readiness review, 2026-09-29

Specification 1.1.0 was checked against the local Makefile, model registration,
snapshot helper and Rack source/API, including the shared-lock serialization
path and synchronous history callbacks. This supersedes the original statement
that no checkout was inspected. No original firmware evidence was changed.

Checks executed for this revision:

- `python3 tools/check_handoff.py`: original reference consistency and refreshed
  package integrity, plus the new review checker.
- `python3 tools/check_review_contract.py`: 24 exact rational P-policy examples,
  153 unique acceptance IDs, all 26 navigation links, three valid JSON examples
  and all 13 implementation packets. These acceptance IDs describe future
  implementation tests; they are not 153 executed C++ module tests.
- WSL GCC compilation/run of `tools/spec_kernel_smoke.cpp` using
  `-std=c++11 -O2 -Wall -Wextra -Werror -fsanitize=undefined
  -fno-sanitize-recover=all`: passed all 1,743 ratio, 42 scalar phase and 64 PRNG
  rows without UBSan errors. This explicitly checks the repository's production
  language standard rather than relying only on the earlier C++17 run.
- Original reference files, recovered fixtures, default payload, existing
  policy examples and original-harness log retain their prior SHA-256 values.

The new policy vectors are intentionally separate from recovered fixtures.
No native Tempi plugin, audio engine, Rack panel or physical TEMPI was tested
because this task revised a handoff, not an implementation. Earlier reference
harness totals retain their original limits. A full fresh timing-harness run
was not attempted. The unrelated baseline Sibyl P5/P6 failures are noted in
the implementation plan so an implementer does not misattribute them to Tempi.
