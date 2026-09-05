#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MPI_ROOT="${ROOT_DIR}/third_party/openmpi"

export PATH="${MPI_ROOT}/bin:${PATH}"
export DYLD_LIBRARY_PATH="${MPI_ROOT}/lib:${DYLD_LIBRARY_PATH:-}"
export OPAL_PREFIX="${MPI_ROOT}"

exec "${ROOT_DIR}/build/debug/bin/opencagd_hello" "$@"
