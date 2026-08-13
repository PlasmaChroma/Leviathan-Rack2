#!/usr/bin/env bash
set -Eeuo pipefail

# cppcheck-rack.sh
#
# Version-adaptive Cppcheck helper for Make/Bear projects such as VCV Rack plugins.
#
# Design:
#   - Capture the REAL compiler configuration with Bear.
#   - Ask the installed Cppcheck what options it supports.
#   - Use newer analysis features when available.
#   - Remain usable with older releases such as Cppcheck 2.7.
#
# Normal use:
#   ./cppcheck-rack.sh
#
# Fresh compile database:
#   ./cppcheck-rack.sh --rebuild-db
#
# Strongest analysis supported by the installed Cppcheck:
#   ./cppcheck-rack.sh --deep
#
# Approximate the curated Cppcheck pass used by Rack maintainers:
#   ./cppcheck-rack.sh --maintainer
#
# Explore alternate #ifdef configurations too:
#   ./cppcheck-rack.sh --all-configs
#
# Produce XML as well:
#   ./cppcheck-rack.sh --xml

die() {
    echo "ERROR: $*" >&2
    exit 1
}

note() {
    echo "NOTE: $*" >&2
}

have() {
    command -v "$1" >/dev/null 2>&1
}

usage() {
    cat <<'EOF'
Usage: cppcheck-rack.sh [options]

Compilation database:
  --rebuild-db       Force make clean + Bear rebuild.
  --reuse-db         Require/reuse the existing compile_commands.json.

Analysis:
  --maintainer       Approximate Rack's maintainer-facing Cppcheck profile:
                     normal depth, captured build configuration, and the
                     warning/performance/portability groups. Suppresses
                     diagnostics originating in the Rack SDK and bundled
                     third-party sources, plus copy-semantics noise from
                     Rack's parent-owned widget pointer pattern.
  --deep             Use the strongest deeper-analysis switch supported.
                     Cppcheck >= 2.11: --check-level=exhaustive
                     Older Cppcheck: no fake equivalent is added.
  --all-configs      Explore all reachable #ifdef configurations with --force.
  --max-configs N    Use --max-configs=N only if this Cppcheck supports it.
                     If unsupported, analysis continues without it.
  --xml              Also generate cppcheck.xml (second analysis pass).
  --clean-cache      Remove the Cppcheck analysis cache first.

General:
  -j, --jobs N       Build/analysis parallelism. Default: CPU count.
  -o, --output DIR   Output directory. Default: .analysis/cppcheck
  --show-capabilities
                     Print detected Cppcheck capabilities and exit.
  -h, --help         Show this help.

Environment overrides:
  MAKE                Make executable (default: make)
  BEAR                Bear executable (default: bear)
  CPPCHECK            Cppcheck executable (default: cppcheck)
EOF
}

# Find repository root.
if have git && git rev-parse --show-toplevel >/dev/null 2>&1; then
    ROOT="$(git rev-parse --show-toplevel)"
else
    ROOT="$(pwd)"
fi

MAKE_BIN="${MAKE:-make}"
BEAR_BIN="${BEAR:-bear}"
CPPCHECK_BIN="${CPPCHECK:-cppcheck}"

if have nproc; then
    JOBS="$(nproc)"
else
    JOBS=1
fi

OUTPUT_DIR="$ROOT/.analysis/cppcheck"
DB="$ROOT/compile_commands.json"

DB_MODE="auto"       # auto | rebuild | reuse
DEEP=0
ALL_CONFIGS=0
MAX_CONFIGS=""
MAKE_XML=0
CLEAN_CACHE=0
SHOW_CAPABILITIES=0
MAINTAINER=0
OUTPUT_EXPLICIT=0

while (($#)); do
    case "$1" in
        --rebuild-db)
            DB_MODE="rebuild"
            shift
            ;;
        --reuse-db)
            DB_MODE="reuse"
            shift
            ;;
        --deep)
            DEEP=1
            shift
            ;;
        --maintainer)
            MAINTAINER=1
            shift
            ;;
        --all-configs)
            ALL_CONFIGS=1
            shift
            ;;
        --max-configs)
            [[ $# -ge 2 ]] || die "--max-configs requires a number"
            MAX_CONFIGS="$2"
            [[ "$MAX_CONFIGS" =~ ^[1-9][0-9]*$ ]] ||
                die "--max-configs must be a positive integer"
            shift 2
            ;;
        --xml)
            MAKE_XML=1
            shift
            ;;
        --clean-cache)
            CLEAN_CACHE=1
            shift
            ;;
        --show-capabilities)
            SHOW_CAPABILITIES=1
            shift
            ;;
        -j|--jobs)
            [[ $# -ge 2 ]] || die "$1 requires a number"
            JOBS="$2"
            [[ "$JOBS" =~ ^[1-9][0-9]*$ ]] ||
                die "--jobs must be a positive integer"
            shift 2
            ;;
        -o|--output)
            [[ $# -ge 2 ]] || die "$1 requires a directory"
            OUTPUT_DIR="$2"
            OUTPUT_EXPLICIT=1
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            die "Unknown argument: $1 (use --help)"
            ;;
    esac
done

if (( MAINTAINER )); then
    (( DEEP == 0 )) || die "--maintainer cannot be combined with --deep"
    (( ALL_CONFIGS == 0 )) || die "--maintainer cannot be combined with --all-configs"
    [[ -z "$MAX_CONFIGS" ]] || die "--maintainer cannot be combined with --max-configs"
    if (( OUTPUT_EXPLICIT == 0 )); then
        OUTPUT_DIR="$ROOT/.analysis/cppcheck-maintainer"
    fi
fi

have "$CPPCHECK_BIN" || die "Cannot find Cppcheck executable: $CPPCHECK_BIN"
have "$MAKE_BIN"     || die "Cannot find make executable: $MAKE_BIN"
have "$BEAR_BIN"     || die "Cannot find Bear executable: $BEAR_BIN"
have python3         || die "python3 is required to validate compile_commands.json"

CPPCHECK_VERSION="$("$CPPCHECK_BIN" --version 2>&1 | head -n 1)"
CPPCHECK_HELP="$("$CPPCHECK_BIN" --help 2>&1 || true)"

# Capability detection is intentionally based on the installed executable's
# help output rather than a hard-coded version table.
has_cppcheck_option() {
    grep -Fq -- "$1" <<<"$CPPCHECK_HELP"
}

capability() {
    local option="$1"
    local label="$2"
    if has_cppcheck_option "$option"; then
        printf "  %-32s yes\n" "$label"
    else
        printf "  %-32s no\n" "$label"
    fi
}

print_capabilities() {
    echo "$CPPCHECK_VERSION"
    echo "Detected capabilities:"
    capability "--project"              "compile database (--project)"
    capability "--cppcheck-build-dir"   "analysis cache/build dir"
    capability "--check-level"          "check-level / exhaustive"
    capability "--force"                "all #ifdef configs (--force)"
    capability "--max-configs"          "bounded #ifdef configs"
    capability "--inline-suppr"         "inline suppressions"
    capability "--template"             "output templates"
    capability "--error-exitcode"       "finding exit code"
    capability "--xml"                  "XML output"
    capability "--enable"               "check groups (--enable)"
}

if (( SHOW_CAPABILITIES )); then
    print_capabilities
    exit 0
fi

# compile_commands.json is the central requirement: it provides Cppcheck with
# the actual -D, -I, standard, architecture and compiler configuration used by
# the build rather than a guessed imitation of it.
has_cppcheck_option "--project" ||
    die "$CPPCHECK_VERSION does not advertise --project support; cannot safely use the compilation-database workflow."

cd "$ROOT"
mkdir -p "$OUTPUT_DIR"

CACHE_DIR="$OUTPUT_DIR/cache"
TEXT_REPORT="$OUTPUT_DIR/cppcheck.txt"
RAW_REPORT="$OUTPUT_DIR/cppcheck.raw.txt"
XML_REPORT="$OUTPUT_DIR/cppcheck.xml"
RUN_INFO="$OUTPUT_DIR/run-info.txt"

db_entry_count() {
    python3 - "$DB" <<'PY'
import json
import sys
from pathlib import Path

p = Path(sys.argv[1])
if not p.is_file():
    print(0)
    raise SystemExit

try:
    data = json.loads(p.read_text(encoding="utf-8"))
except Exception:
    print(0)
    raise SystemExit

print(len(data) if isinstance(data, list) else 0)
PY
}

validate_db() {
    local count
    count="$(db_entry_count)"
    if [[ "$count" -le 0 ]]; then
        return 1
    fi
    echo "$count"
}

# Find Rack SDK roots from the captured compiler include paths. Cppcheck still
# parses these headers for types and control flow, but diagnostics originating
# inside the SDK are external to the plugin and should not dominate its report.
discover_rack_sdk_roots() {
    python3 - "$DB" <<'PY'
import json
import shlex
import sys
from pathlib import Path

with open(sys.argv[1], "r", encoding="utf-8") as f:
    db = json.load(f)

roots = set()
for entry in db:
    directory = Path(entry.get("directory") or ".")
    args = entry.get("arguments")
    if args is None:
        args = shlex.split(entry.get("command", ""))
    i = 0
    while i < len(args):
        arg = args[i]
        include = None
        if arg == "-I" and i + 1 < len(args):
            i += 1
            include = args[i]
        elif arg.startswith("-I") and len(arg) > 2:
            include = arg[2:]
        if include:
            include_path = Path(include)
            if not include_path.is_absolute():
                include_path = directory / include_path
            try:
                include_path = include_path.resolve()
            except OSError:
                pass
            if include_path.name == "include" and (include_path / "rack.hpp").is_file():
                roots.add(str(include_path.parent))
        i += 1

for root in sorted(roots):
    print(root)
PY
}

generate_db() {
    echo "==> Rebuilding compilation database with Bear"
    rm -f "$DB"

    "$MAKE_BIN" clean
    "$BEAR_BIN" -- "$MAKE_BIN" -j"$JOBS"

    local count
    if ! count="$(validate_db)"; then
        cat >&2 <<EOF

Bear did not produce a usable compilation database:
  $DB

If it contains [], Bear saw zero compiler invocations.

Verify that the rebuild visibly compiled your .cpp/.c files.

A useful Bear diagnostic is:
  RUST_LOG=debug $BEAR_BIN -- $MAKE_BIN -j$JOBS 2> bear-debug.log
EOF
        exit 1
    fi

    echo "==> Captured $count translation unit(s)"
}

case "$DB_MODE" in
    rebuild)
        generate_db
        ;;
    reuse)
        if count="$(validate_db)"; then
            echo "==> Reusing compile_commands.json ($count translation unit(s))"
        else
            die "--reuse-db requested, but compile_commands.json is missing, invalid, or empty"
        fi
        ;;
    auto)
        if count="$(validate_db)"; then
            echo "==> Reusing compile_commands.json ($count translation unit(s))"
            echo "    Use --rebuild-db after build flags, SDK configuration, or source membership changes."
        else
            generate_db
        fi
        ;;
esac

# Build the command strictly from capabilities the installed executable reports.
CPPCHECK_ARGS=(
    "--project=$DB"
)

RACK_SDK_ROOTS=()
while IFS= read -r sdk_root; do
    [[ -n "$sdk_root" ]] && RACK_SDK_ROOTS+=("$sdk_root")
done < <(discover_rack_sdk_roots)

if has_cppcheck_option "--suppress"; then
    for sdk_root in "${RACK_SDK_ROOTS[@]}"; do
        CPPCHECK_ARGS+=("--suppress=*:$sdk_root/*")
    done
    if (( MAINTAINER )); then
        CPPCHECK_ARGS+=("--suppress=*:$ROOT/src/third_party/*")
        CPPCHECK_ARGS+=("--suppress=noCopyConstructor")
        CPPCHECK_ARGS+=("--suppress=noOperatorEq")
    fi
elif (( ${#RACK_SDK_ROOTS[@]} )); then
    note "$CPPCHECK_VERSION does not advertise --suppress; Rack SDK diagnostics cannot be filtered."
fi

if has_cppcheck_option "--enable"; then
    if (( MAINTAINER )); then
        CPPCHECK_ARGS+=("--enable=warning,performance,portability")
    else
        CPPCHECK_ARGS+=("--enable=warning,style,performance,portability,information")
    fi
fi

if has_cppcheck_option "--inline-suppr"; then
    CPPCHECK_ARGS+=("--inline-suppr")
fi

if (( MAINTAINER == 0 )) && has_cppcheck_option "--template"; then
    CPPCHECK_ARGS+=("--template=gcc")
fi

if has_cppcheck_option "--error-exitcode"; then
    CPPCHECK_ARGS+=("--error-exitcode=2")
fi

# Cppcheck's persistent build directory is valuable when supported, but old
# installations should not fail merely because the option is unavailable.
if has_cppcheck_option "--cppcheck-build-dir"; then
    if (( CLEAN_CACHE )); then
        echo "==> Removing Cppcheck cache"
        rm -rf "$CACHE_DIR"
    fi
    mkdir -p "$CACHE_DIR"
    CPPCHECK_ARGS+=("--cppcheck-build-dir=$CACHE_DIR")
elif (( CLEAN_CACHE )); then
    note "$CPPCHECK_VERSION has no --cppcheck-build-dir option; --clean-cache has nothing to clean."
fi

# Parallelism has existed for a long time, but still test the executable.
if grep -Eq '(^|[[:space:],])-j([[:space:],=<]|$)' <<<"$CPPCHECK_HELP"; then
    CPPCHECK_ARGS+=("-j$JOBS")
else
    note "$CPPCHECK_VERSION does not advertise -j; running Cppcheck single-threaded."
fi

DEEP_DESCRIPTION="normal/default"
if (( DEEP )); then
    if has_cppcheck_option "--check-level"; then
        CPPCHECK_ARGS+=("--check-level=exhaustive")
        DEEP_DESCRIPTION="exhaustive (--check-level=exhaustive)"
    else
        DEEP_DESCRIPTION="requested, but unavailable in this Cppcheck"
        note "$CPPCHECK_VERSION predates --check-level=exhaustive; continuing with this release's normal analysis."
    fi
fi

CONFIG_DESCRIPTION="captured build configuration"
if (( ALL_CONFIGS )); then
    if has_cppcheck_option "--force"; then
        CPPCHECK_ARGS+=("--force")
        CONFIG_DESCRIPTION="all reachable #ifdef configurations (--force)"
    else
        note "$CPPCHECK_VERSION does not advertise --force; staying on the captured build configuration."
    fi
elif [[ -n "$MAX_CONFIGS" ]]; then
    if has_cppcheck_option "--max-configs"; then
        CPPCHECK_ARGS+=("--max-configs=$MAX_CONFIGS")
        CONFIG_DESCRIPTION="up to $MAX_CONFIGS #ifdef configurations"
    else
        note "$CPPCHECK_VERSION does not support --max-configs; continuing with its default configuration limit."
        note "Use --all-configs if you explicitly want --force and this release supports it."
    fi
fi

# Record exactly what this installation decided to use.
{
    echo "timestamp: $(date --iso-8601=seconds 2>/dev/null || date)"
    echo "repo: $ROOT"
    echo "cppcheck_version: $CPPCHECK_VERSION"
    echo "compile_database: $DB"
    echo "translation_units: $(db_entry_count)"
    echo "jobs_requested: $JOBS"
    if (( MAINTAINER )); then
        echo "analysis_profile: maintainer approximation"
        echo "enabled_groups: warning,performance,portability"
        echo "suppressed_bundled_sources: $ROOT/src/third_party"
        echo "suppressed_ownership_checks: noCopyConstructor,noOperatorEq"
        echo "filtered_type_specific_checks: passedByValue(NVGcolor)"
    else
        echo "analysis_profile: general"
        echo "enabled_groups: warning,style,performance,portability,information"
    fi
    echo "deep_analysis: $DEEP_DESCRIPTION"
    echo "configuration_analysis: $CONFIG_DESCRIPTION"
	if (( ${#RACK_SDK_ROOTS[@]} )); then
		echo "suppressed_rack_sdk_roots: ${RACK_SDK_ROOTS[*]}"
	else
		echo "suppressed_rack_sdk_roots: none detected"
	fi
    echo
    print_capabilities
    echo
    echo "cppcheck command:"
    printf '%q ' "$CPPCHECK_BIN" "${CPPCHECK_ARGS[@]}"
    echo
    echo
    echo "first captured translation unit:"
    python3 - "$DB" <<'PY'
import json
import shlex
import sys

with open(sys.argv[1], "r", encoding="utf-8") as f:
    db = json.load(f)

if db:
    e = db[0]
    print("file:", e.get("file", ""))
    print("directory:", e.get("directory", ""))
    if "arguments" in e:
        print("command:", shlex.join(e["arguments"]))
    else:
        print("command:", e.get("command", ""))
PY
} > "$RUN_INFO"

echo
echo "==> Cppcheck capability selection"
echo "    Version:       $CPPCHECK_VERSION"
if (( MAINTAINER )); then
    echo "    Profile:       maintainer approximation"
    echo "    Check groups:  warning, performance, portability"
    echo "    Bundled code:  suppressed ($ROOT/src/third_party)"
    echo "    Rack ownership: suppress noCopyConstructor, noOperatorEq"
else
    echo "    Profile:       general"
    echo "    Check groups:  warning, style, performance, portability, information"
fi
echo "    Deep analysis: $DEEP_DESCRIPTION"
echo "    Configs:       $CONFIG_DESCRIPTION"
if (( ${#RACK_SDK_ROOTS[@]} )); then
    echo "    SDK findings:  suppressed (${RACK_SDK_ROOTS[*]})"
else
    echo "    SDK findings:  no Rack SDK root detected"
fi
echo
echo "==> Running Cppcheck"
echo "    Report:        $TEXT_REPORT"
if (( MAINTAINER )); then
    echo "    Raw report:    $RAW_REPORT"
fi
echo

set +e
if (( MAINTAINER )); then
    "$CPPCHECK_BIN" "${CPPCHECK_ARGS[@]}" 2>&1 | tee "$RAW_REPORT"
else
    "$CPPCHECK_BIN" "${CPPCHECK_ARGS[@]}" 2>&1 | tee "$TEXT_REPORT"
fi
CPPCHECK_STATUS=${PIPESTATUS[0]}
set -e

FILTERED_NVGCOLOR_COUNT=0
if (( MAINTAINER )); then
    FILTERED_COUNT_FILE="$OUTPUT_DIR/filtered-nvgcolor-count.txt"
    python3 - "$RAW_REPORT" "$TEXT_REPORT" "$FILTERED_COUNT_FILE" <<'PY'
import re
import sys
from pathlib import Path

raw_path = Path(sys.argv[1])
report_path = Path(sys.argv[2])
count_path = Path(sys.argv[3])
lines = raw_path.read_text(encoding="utf-8", errors="replace").splitlines(keepends=True)
diagnostic = re.compile(
    r"^(.*):(\d+):(\d+):\s+\w+:\s+Function parameter '([^']+)' .*\[passedByValue\]\s*$"
)

def is_nvgcolor_parameter(filename, line_number, parameter):
    try:
        source_line = Path(filename).read_text(encoding="utf-8", errors="replace").splitlines()[line_number - 1]
    except (OSError, IndexError):
        return False
    pattern = r"\bNVGcolor\s+(?:const\s+)?" + re.escape(parameter) + r"\b"
    return re.search(pattern, source_line) is not None

filtered = 0
output = []
i = 0
while i < len(lines):
    match = diagnostic.match(lines[i].rstrip("\r\n"))
    if match and is_nvgcolor_parameter(match.group(1), int(match.group(2)), match.group(4)):
        filtered += 1
        i += 1
        # Default Cppcheck output follows a diagnostic with its source line and
        # caret. Remove only those two presentation lines when present.
        for _ in range(2):
            if i < len(lines) and not diagnostic.match(lines[i].rstrip("\r\n")):
                i += 1
        continue
    output.append(lines[i])
    i += 1

report_path.write_text("".join(output), encoding="utf-8")
count_path.write_text(str(filtered), encoding="ascii")
PY
    FILTERED_NVGCOLOR_COUNT="$(cat "$FILTERED_COUNT_FILE")"
fi

if (( MAKE_XML )); then
    if has_cppcheck_option "--xml"; then
        echo
        echo "==> Producing XML report (second Cppcheck pass)"

        XML_ARGS=()
        for arg in "${CPPCHECK_ARGS[@]}"; do
            # Human-readable templates and finding-specific exit codes are not
            # useful for the XML pass.
            [[ "$arg" == "--template=gcc" ]] && continue
            [[ "$arg" == "--error-exitcode=2" ]] && continue
            XML_ARGS+=("$arg")
        done

        set +e
        "$CPPCHECK_BIN" "${XML_ARGS[@]}" --xml 2> "$XML_REPORT" >/dev/null
        XML_STATUS=$?
        set -e

        if [[ "$XML_STATUS" -ne 0 ]]; then
            note "XML pass exited with status $XML_STATUS; inspect $XML_REPORT."
        fi
    else
        note "$CPPCHECK_VERSION does not advertise --xml; no XML report generated."
    fi
fi

echo
echo "==> Analysis complete"
echo "    Text report: $TEXT_REPORT"
if (( MAINTAINER )); then
    echo "    Raw report:  $RAW_REPORT"
    echo "    Filtered:    $FILTERED_NVGCOLOR_COUNT passedByValue(NVGcolor) finding(s)"
fi
echo "    Run info:    $RUN_INFO"
if (( MAKE_XML )) && [[ -f "$XML_REPORT" ]]; then
    echo "    XML report:  $XML_REPORT"
fi

echo
echo "Severity counts (best effort):"
for severity in error warning style performance portability information; do
    # Works with common gcc/default Cppcheck formatting.
    count="$(
        grep -Eic \
            "([[:space:]:\(])${severity}([[:space:]:\)])" \
            "$TEXT_REPORT" 2>/dev/null || true
    )"
    printf "  %-12s %s\n" "$severity" "$count"
done

echo
if has_cppcheck_option "--error-exitcode"; then
    if (( MAINTAINER )) && [[ "$CPPCHECK_STATUS" -eq 2 ]] &&
        ! grep -Eq ':[[:space:]]+(error|warning|style|performance|portability|information):' "$TEXT_REPORT"; then
        CPPCHECK_STATUS=0
    fi
    case "$CPPCHECK_STATUS" in
        0)
            echo "Cppcheck exited 0: no enabled finding triggered the configured error exit code."
            ;;
        2)
            echo "Cppcheck exited 2: findings were reported (--error-exitcode=2)."
            ;;
        *)
            echo "Cppcheck exited $CPPCHECK_STATUS; inspect the report for a tool/configuration failure." >&2
            ;;
    esac
else
    echo "Cppcheck exited $CPPCHECK_STATUS."
fi

exit "$CPPCHECK_STATUS"
