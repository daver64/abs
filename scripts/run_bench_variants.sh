#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
CSV_PATH="${ROOT_DIR}/benchmarks/results.csv"

if [[ "${1:-}" == "--help" || "${1:-}" == "-h" ]]; then
    cat << 'EOF'
Usage: scripts/run_bench_variants.sh [MODE ...]

Runs abs benchmarks for each SIMD mode and writes consolidated CSV.
Modes default to: SCALAR AUTO SSE2 AVX2 NEON

Examples:
  scripts/run_bench_variants.sh
  scripts/run_bench_variants.sh SCALAR AUTO AVX2
EOF
    exit 0
fi

if [[ "$#" -gt 0 ]]; then
    MODES=("$@")
else
    MODES=(SCALAR AUTO SSE2 AVX2 NEON)
fi

mkdir -p "$(dirname "${CSV_PATH}")"
: > "${CSV_PATH}"

header_written=0
any_success=0

for mode in "${MODES[@]}"; do
    echo "[abs bench] mode=${mode}"

    if ! cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
        -DABS_BUILD_BENCHMARKS=ON \
        -DABS_BUILD_DEMO=OFF \
        -DABS_SIMD_MODE="${mode}"; then
        echo "  configure failed, skipping ${mode}" >&2
        continue
    fi

    if ! cmake --build "${BUILD_DIR}" --target abs_bench; then
        echo "  build failed, skipping ${mode}" >&2
        continue
    fi

    if [[ "${header_written}" -eq 0 ]]; then
        if ! "${BUILD_DIR}/abs_bench" --csv --mode "${mode}" >> "${CSV_PATH}"; then
            echo "  benchmark run failed, skipping ${mode}" >&2
            continue
        fi
        header_written=1
    else
        if ! "${BUILD_DIR}/abs_bench" --csv --csv-no-header --mode "${mode}" >> "${CSV_PATH}"; then
            echo "  benchmark run failed, skipping ${mode}" >&2
            continue
        fi
    fi

    any_success=1
    echo "  ok"
done

if [[ "${any_success}" -eq 0 ]]; then
    echo "No benchmark modes succeeded." >&2
    exit 1
fi

echo
echo "Benchmark CSV written to: ${CSV_PATH}"
cat "${CSV_PATH}"
