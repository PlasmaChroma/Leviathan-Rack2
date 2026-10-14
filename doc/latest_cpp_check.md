# Latest Cppcheck Notes

Date: 2026-08-27

## Analysis configuration

The latest full analysis used the Snap build of Cppcheck with the project's
captured compilation database:

```bash
CPPCHECK=/snap/bin/cppcheck ./tools/cppcheck-rack.sh --clean-cache
```

- Cppcheck reported its version as `Cppcheck 2.14 dev`.
- The Snap store stable channel was labelled `2.13.99` when installed.
- `compile_commands.json` contained 121 translation units.
- The old Cppcheck 2.7 cache was removed before this run.
- The general profile enabled warning, style, performance, portability, and
  information checks using the captured build configuration.
- Rack SDK findings were suppressed by the analysis helper.

The Snap executable must currently be selected explicitly with
`CPPCHECK=/snap/bin/cppcheck`; otherwise the helper can resolve the distro
Cppcheck 2.7 binary.

## Environment limitation

Snap confinement prevented Cppcheck from seeing host standard-library and
system headers. This generated a large number of `missingIncludeSystem`
messages even though the compilation database and Rack SDK paths were loaded.
Cppcheck states that standard-library headers are not required for most of its
analysis, but missing preprocessing context can still distort value-flow
results. New control-flow findings from this run should therefore be audited,
not accepted automatically.

The report also includes bundled dependency diagnostics from
`src/third_party`. Those are excluded from the first-party triage below.

## Raw result

- Cppcheck exit status: 2, meaning enabled findings were reported.
- Raw summary: 4 errors and 1810 warnings.
- Actual diagnostics formatted as errors: 0.
- The four-error summary is a best-effort report-counting artifact.

Full report:

```text
.analysis/cppcheck/cppcheck.txt
```

Run metadata:

```text
.analysis/cppcheck/run-info.txt
```

## Filtered first-party result

After excluding `src/third_party`, Rack SDK diagnostics,
`missingIncludeSystem`, and `checkersReport`, 732 first-party diagnostics
remained:

| Count | Cppcheck ID |
| ---: | --- |
| 366 | `constVariablePointer` |
| 83 | `passedByValue` |
| 82 | `useStlAlgorithm` |
| 53 | `constParameterPointer` |
| 19 | `knownConditionTrueFalse` |
| 17 | `shadowFunction` |
| 17 | `noOperatorEq` |
| 17 | `noCopyConstructor` |
| 15 | `noExplicitConstructor` |
| 11 | `variableScope` |
| 9 | `constVariableReference` |
| 8 | `unusedStructMember` |
| 7 | `shadowVariable` |
| 7 | `cstyleCast` |
| 6 | `duplicateCondition` |
| 5 | `checkLevelNormal` |
| 4 | `uselessOverride` |
| 3 | `duplInheritedMember` |
| 1 | `unusedPrivateFunction` |
| 1 | `redundantCondition` |
| 1 | `constParameterReference` |

Most of this total is low-value const, parameter-passing, STL-style, and Rack
widget ownership noise. It should not be treated as 732 correctness defects.

## Confirmed fixes made before the latest run

- Added a defensive upper-bound fallback in `MoiraiCurves.hpp` before
  indexing a contour point. The previous `containerOutOfBounds` finding is no
  longer present.
- Enlarged Sibyl scene and repeat status buffers from 24 to 32 bytes. The
  compiler truncation warning is no longer present.
- Changed `OctaviaConsoleMailbox::setError()` to accept
  `const std::string&`. The maintainer-profile `passedByValue` finding for that
  function is no longer present.
- Removed an unread Moirai assignment.
- Removed redundant SVG tag-size predicates where an enclosing condition
  already guaranteed the minimum size.
- Combined adjacent identical Temporal Deck guards.
- Simplified a Temporal Deck menu comparator, a Sil histogram bound check, a
  Deepcache preview reset, and plugin JSON boolean handling without intended
  behavior changes.

## Remaining correctness candidates

### Deepcache archive replacement

`DeepcacheArchive.cpp` around lines 1433-1448 reports that
`indexBackedUp` is always false and `indexInstalled` is always false. This is
the highest-value new item to inspect. It may indicate that filesystem
replacement branches were removed or disabled, or it may be distorted by the
Snap build's missing system-header context.

### Panel SVG parsing

`PanelSvgUtils.cpp` reports:

- the failure branch after `appendSvgArcAsCubics()` is unreachable;
- the `id` side of `!id.empty() ? id : label` is unreachable in one matcher.

These should be checked against parser invariants before simplifying them.

### Temporal Deck

The analyzer still reports several duplicated or constant conditions in
Temporal Deck and its UI. The adjacent `!loopActive` duplication was already
removed. Remaining scratch-path warnings are not safe to rewrite without
deciding which manual, touch, wheel, or legacy mode the branch was intended to
serve.

The `menuVisibleJ` redundant-condition warning is caused by Jansson boolean
macro expansion and is not a behavioral defect.

### Worker concurrency

The `DeepcachePlanner.cpp` stale-job checks and `Iris.cpp` intermediate-result
checks are expected concurrency false positives. Cppcheck reasons within the
worker thread but does not account for another thread changing the guarded
state while the worker is unlocked.

### Duplicate performance guards

Several `duplicateCondition` reports guard distinct instrumentation operations
with the same condition, including Wyrm GL timer query transitions and UI
performance measurements. Identical guard expressions separated by different
operations are not automatically redundant and should not be merged unless
the complete timing sequence remains unchanged.

## Doorstop status

The latest run produced no Doorstop engine correctness diagnostic. Doorstop
serialization received only const-pointer suggestions. The V3 helical engine
did not produce a new behavioral warning.

## Maintainer-profile context

The newer Cppcheck maintainer profile was also run separately. Its first-party
output consisted of Jansson `json_object_foreach`/`json_array_foreach`
`unknownMacro` reports and the now-fixed mailbox parameter warning. The
maintainer profile scans source directly rather than loading the compilation
database, so it does not receive the dependency context needed to expand the
Jansson macros. There are 39 idiomatic Jansson foreach uses in first-party
source; rewriting all of them for this profile would be unjustified churn.

For repository triage, prefer the general compilation-database profile and
filter the known Snap/header and third-party noise.
