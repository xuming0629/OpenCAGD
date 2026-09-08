#!/usr/bin/env bash
set -euo pipefail

PRESET="${1:-debug}"

case "$PRESET" in
  debug|debug-plot|debug-vtk|debug-all|release-shared) ;;
  *)
    echo "Usage: $0 [debug|debug-plot|debug-vtk|debug-all|release-shared]" >&2
    exit 2
    ;;
esac

cmake --preset "$PRESET"
cmake --build --preset "$PRESET" -j

if [[ "$PRESET" == "debug" || "$PRESET" == "debug-plot" ]]; then
  ctest --preset "$PRESET"
fi
