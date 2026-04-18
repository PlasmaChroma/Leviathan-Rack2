#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  cat <<'USAGE'
Usage:
  scripts/bifurx_curve_debug_summary.sh <curve_debug_csv> [more_csvs...]

Notes:
  - Expects Bifurx curve debug trace v3 CSV files.
  - Summarizes LL-only rows (`ll_active == 1`) by `circuit_mode` and `mode`.
USAGE
  exit 1
fi

awk -F, '
function trim(s) {
  gsub(/^[ \t\r\n]+|[ \t\r\n]+$/, "", s)
  return s
}

function circuit_label(mode) {
  if (mode == 0) return "SVF"
  if (mode == 1) return "DFM"
  if (mode == 2) return "MS2"
  if (mode == 3) return "PRD"
  return "UNKNOWN"
}

function mode_label(mode) {
  if (mode == 0) return "Low + Low"
  if (mode == 1) return "Low + Band"
  if (mode == 2) return "Notch + Low"
  if (mode == 3) return "Notch + Notch"
  if (mode == 4) return "Low + High"
  if (mode == 5) return "Band + Band"
  if (mode == 6) return "High + Low"
  if (mode == 7) return "High + Notch"
  if (mode == 8) return "Band + High"
  if (mode == 9) return "High + High"
  return "UNKNOWN"
}

function update_minmax(v, key, is_first) {
  if (is_first || v < minv[key]) minv[key] = v
  if (is_first || v > maxv[key]) maxv[key] = v
}

BEGIN {
  OFS = "\t"
}

/^#/ { next }

$1 == "seq" {
  delete col
  for (i = 1; i <= NF; ++i) {
    name = trim($i)
    col[name] = i
  }
  required = 1
  if (!("circuit_mode" in col)) required = 0
  if (!("ll_active" in col)) required = 0
  if (!("ll_input_rms" in col)) required = 0
  if (!("ll_stage_a_lp_rms" in col)) required = 0
  if (!("ll_stage_b_lp_rms" in col)) required = 0
  if (!("ll_output_rms" in col)) required = 0
  if (!("ll_stage_b_over_a_db" in col)) required = 0
  if (!("ll_output_over_input_db" in col)) required = 0
  next
}

{
  if (!required) next

  ll_active = trim($(col["ll_active"])) + 0
  if (ll_active != 1) next

  circuit = trim($(col["circuit_mode"])) + 0
  mode = trim($(col["mode"])) + 0
  key = circuit SUBSEP mode
  seenCircuit[circuit] = 1
  seenMode[mode] = 1

  v_in = trim($(col["ll_input_rms"])) + 0
  v_a  = trim($(col["ll_stage_a_lp_rms"])) + 0
  v_b  = trim($(col["ll_stage_b_lp_rms"])) + 0
  v_o  = trim($(col["ll_output_rms"])) + 0
  v_ba = trim($(col["ll_stage_b_over_a_db"])) + 0
  v_oi = trim($(col["ll_output_over_input_db"])) + 0

  count[key]++
  sum_in[key] += v_in
  sum_a[key] += v_a
  sum_b[key] += v_b
  sum_o[key] += v_o
  sum_ba[key] += v_ba
  sum_oi[key] += v_oi

  first = (count[key] == 1)
  update_minmax(v_ba, key "_ba", first)
  update_minmax(v_oi, key "_oi", first)
}

END {
  print "circuit_mode", "label", "rows", "avg_ll_input_rms", "avg_ll_stage_a_lp_rms", "avg_ll_stage_b_lp_rms", "avg_ll_output_rms", "avg_ll_stage_b_over_a_db", "avg_ll_output_over_input_db", "min_ll_stage_b_over_a_db", "max_ll_stage_b_over_a_db", "min_ll_output_over_input_db", "max_ll_output_over_input_db"
  for (circuit = 0; circuit <= 3; ++circuit) {
    c = 0
    for (mode = 0; mode <= 9; ++mode) {
      key = circuit SUBSEP mode
      c += count[key] + 0
    }
    if (c == 0) {
      print circuit, circuit_label(circuit), 0, "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"
      continue
    }
    sum_in_c = 0
    sum_a_c = 0
    sum_b_c = 0
    sum_o_c = 0
    sum_ba_c = 0
    sum_oi_c = 0
    min_ba_c = ""
    max_ba_c = ""
    min_oi_c = ""
    max_oi_c = ""
    first = 1
    for (mode = 0; mode <= 9; ++mode) {
      key = circuit SUBSEP mode
      rows = count[key] + 0
      if (rows == 0) continue
      sum_in_c += sum_in[key]
      sum_a_c += sum_a[key]
      sum_b_c += sum_b[key]
      sum_o_c += sum_o[key]
      sum_ba_c += sum_ba[key]
      sum_oi_c += sum_oi[key]
      if (first || minv[key "_ba"] < min_ba_c) min_ba_c = minv[key "_ba"]
      if (first || maxv[key "_ba"] > max_ba_c) max_ba_c = maxv[key "_ba"]
      if (first || minv[key "_oi"] < min_oi_c) min_oi_c = minv[key "_oi"]
      if (first || maxv[key "_oi"] > max_oi_c) max_oi_c = maxv[key "_oi"]
      first = 0
    }
    print circuit, circuit_label(circuit), c, \
      sum_in_c / c, sum_a_c / c, sum_b_c / c, sum_o_c / c, \
      sum_ba_c / c, sum_oi_c / c, \
      min_ba_c, max_ba_c, min_oi_c, max_oi_c
  }
  print ""
  print "circuit_mode", "circuit_label", "mode", "mode_label", "rows", "avg_ll_input_rms", "avg_ll_stage_a_lp_rms", "avg_ll_stage_b_lp_rms", "avg_ll_output_rms", "avg_ll_stage_b_over_a_db", "avg_ll_output_over_input_db", "min_ll_stage_b_over_a_db", "max_ll_stage_b_over_a_db", "min_ll_output_over_input_db", "max_ll_output_over_input_db"
  for (circuit = 0; circuit <= 3; ++circuit) {
    for (mode = 0; mode <= 9; ++mode) {
      key = circuit SUBSEP mode
      c = count[key] + 0
      if (c == 0) {
        print circuit, circuit_label(circuit), mode, mode_label(mode), 0, "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"
        continue
      }
      print circuit, circuit_label(circuit), mode, mode_label(mode), c, \
        sum_in[key] / c, sum_a[key] / c, sum_b[key] / c, sum_o[key] / c, \
        sum_ba[key] / c, sum_oi[key] / c, \
        minv[key "_ba"], maxv[key "_ba"], minv[key "_oi"], maxv[key "_oi"]
    }
  }
}
' "$@"
