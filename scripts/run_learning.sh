#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

PRESET="${OPENCAGD_PRESET:-debug}"
cmake --preset "${PRESET}"
cmake --build --preset "${PRESET}" --parallel

BIN="${ROOT_DIR}/build/${PRESET}/bin"

"${BIN}/opencagd_ch02_bernstein"
"${BIN}/opencagd_ch02_de_casteljau"
"${BIN}/opencagd_ch03_bspline_basis"
"${BIN}/opencagd_ch04_nurbs_circle"

if [[ -x "${BIN}/opencagd_plot_nurbs_circle" ]]; then
  mkdir -p outputs
  "${BIN}/opencagd_plot_nurbs_circle"
  echo "Generated: outputs/nurbs_quarter_circle.png"
fi
