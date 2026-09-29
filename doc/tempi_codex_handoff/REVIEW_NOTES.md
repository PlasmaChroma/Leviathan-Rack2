# Implementation-readiness review — 2026-09-29

The original 1.0 specification was thorough about features and evidence but
left several cross-component decisions to the implementing agent. Revision
1.1 makes those decisions explicit and supplies a packet-by-packet work order.
It remains a specification; there is no Tempi module to claim as validated.

## Material corrections

1. **Serialization ownership:** inspected Rack `Engine.cpp` uses shared locks
   for both processing and saving. Saving now requires a separate coherent
   persistence exchange; it cannot read the mutable core or share the widget's
   single-consumer snapshot. Exclusive load/reset and published initialization
   are distinguished from concurrent save.
2. **Undo delivery:** Rack Action callbacks cannot defer history-stack movement.
   The contract now specifies a synchronous, exclusive, masked restore bridge,
   without a best-effort queue or executing commands embedded in patch JSON.
   Before/after recording remains bounded and reserved before mutation.
3. **Tempo and Run:** corrected the contradiction about H-only changes clearing
   displacement. Both master-relative and tick-relative representations survive
   H-only changes; committed channel edits clear them. Shift retains displacement
   relative to the original beta, rather than incorrectly retaining only a start
   timestamp.
4. **Timing boundaries:** specified sample timestamps, initialization, strict
   next-rise commit parity, fixed guard fallback deadlines, HIGH preservation,
   shared H generations, pulse-end equality, conversion remainder preservation
   and bounded emergency catch-up. Low-frequency edge rounding now has concrete
   examples rather than relying on a model to guess loop order.
5. **Capture/arbitration:** specified capture producer reset/reacquisition and
   selection of one valid winning tempo candidate before mutation. This avoids
   applying a losing tap correction before checking an equal-H physical capture.
6. **Gestures and selector:** fixed threshold equality, chord/hold ownership,
   delayed Human classification, page-local precedence, menu-opened State Edit,
   panel/keyboard OR ownership and separate observed knob versus selected base.
7. **Command ordering:** explicit memory operations remain FIFO. Bus select/copy
   sequences read the correct provisional State. Automatic selector priority
   cannot accidentally adopt the Bank of a losing bus request. Reset invalidates
   old queued generations; a same-target automatic request is not a reactivation.
8. **Missing evidence:** the 4,096 ADC cases and historical six-lane/timing totals
   are aggregate harness reports, not complete bundled input vectors. Acceptance
   now names the executable rows and additional boundary/reference tests honestly.
9. **Actual integration:** documented nonrecursive source discovery, production
   C++11, unsafe-math overrides, model registration, SVG/anchor helpers, debug
   timing contract, native Windows validation and existing broad-suite failures.
10. **Persistence recovery:** defined malformed scalar/array behavior and preserving
    unsupported JSON verbatim through autosave until explicit replacement.

## Added implementation aids

- `IMPLEMENTATION_PLAN.md`: T00–T12 with read sections, files, proposed Makefile
  targets, test gates and a required resumable status record.
- Main spec: 153 unique acceptance IDs, including 21 new boundary/ownership gates.
- `fixtures/review_policy_vectors.json`: 24 P-only arithmetic/timing examples.
- `tools/check_review_contract.py`: exact `Fraction` checks plus navigation,
  JSON-example and plan completeness checks, included by `check_handoff.py`.

Original files under `reference/`, the recovered fixtures, the existing default
payload and existing policy examples were left unchanged. Package checksums were
refreshed for the edited documentation/tools and new files. Specification1.1
clarifies the draft `TempiPoliciesV1`; no released patch-format migration is
invented. Future behavior changes after implementation need a new policy version.

Remaining uncertainty is the original fidelity register, not permission to omit
features. Physical-device equivalence, live Rack behavior and future C++ engine
correctness still require their own validation. See `VALIDATION_NOTES.md` for
exact checks executed during this document review.
