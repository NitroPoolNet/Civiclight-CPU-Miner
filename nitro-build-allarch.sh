#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="${1:-${ROOT_DIR}/build/civiclight-miner-linux-x86_64/bin}"
JOBS="${JOBS:-$(nproc)}"
WORK_DIR="$(mktemp -d "${TMPDIR:-/tmp}/civiclight-build.XXXXXX")"

cleanup() {
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

mkdir -p "$OUTPUT_DIR"
cd "$ROOT_DIR"

build_target() {
    local binary_name="$1"
    local cflags="$2"
    local target_source="$WORK_DIR/$binary_name"

    echo
    echo "Building ${binary_name}"
    echo "CFLAGS=${cflags}"

    mkdir -p "$target_source"
    tar \
        --exclude='.git' \
        --exclude='.deps' \
        --exclude='autom4te.cache' \
        --exclude='build' \
        --exclude='release' \
        --exclude='*.o' \
        --exclude='*.lo' \
        --exclude='*.a' \
        --exclude='./cpuminer' \
        --exclude='./cpuminer-avx*' \
        --exclude='./cpuminer-core2' \
        --exclude='./cpuminer-zen*' \
        -C "$ROOT_DIR" -cf - . | tar -C "$target_source" -xf -

    cd "$target_source"
    CFLAGS="$cflags" ./configure --with-curl
    make clean
    make -j"$JOBS"
    strip -s cpuminer
    install -m 0755 cpuminer "$OUTPUT_DIR/$binary_name"

    if ! strings "$OUTPUT_DIR/$binary_name" | grep -x 'civiclight' >/dev/null; then
        echo "ERROR: ${binary_name} was built without CivicLight support" >&2
        exit 1
    fi

    cd "$ROOT_DIR"
}

echo "Preparing CivicLight multi-CPU Linux build"
if [[ "${SKIP_AUTOGEN:-0}" != "1" ]]; then
    ./autogen.sh
fi

# Oldest supported profile. The upstream miner requires a Core 2-class CPU;
# its nominal generic x86-64 profile does not link cleanly in this source tree.
build_target "cpuminer-core2" \
    "-O3 -march=core2 -Wall"

# Sandy Bridge / Ivy Bridge and equivalent AVX+AES CPUs.
build_target "cpuminer-avx" \
    "-O3 -march=corei7-avx -maes -Wall"

# Haswell-era Intel and compatible AVX2+AES CPUs.
build_target "cpuminer-avx2" \
    "-O3 -march=core-avx2 -maes -Wall"

# Cross-vendor AVX2+AES+SHA build.
build_target "cpuminer-avx2-sha" \
    "-O3 -march=core-avx2 -maes -msha -Wall"

# Intel Skylake-X/Cascade Lake class AVX-512 CPUs.
build_target "cpuminer-avx512" \
    "-O3 -march=skylake-avx512 -maes -Wall"

# Cross-vendor AVX-512+AES+SHA build for newer Intel and AMD CPUs.
build_target "cpuminer-avx512-sha" \
    "-O3 -march=skylake-avx512 -maes -msha -Wall"

echo
echo "Built CivicLight binaries in: $OUTPUT_DIR"
