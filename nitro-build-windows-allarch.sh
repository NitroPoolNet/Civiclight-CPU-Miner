#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="${1:-${ROOT_DIR}/build/civiclight-miner-windows-x86_64}"
TARGET="${TARGET:-x86_64-w64-mingw32}"
CROSS_PREFIX="${CROSS_PREFIX:-/opt/civiclight-windows}"
JOBS="${JOBS:-$(nproc)}"
WORK_DIR="$(mktemp -d "${TMPDIR:-/tmp}/civiclight-windows-build.XXXXXX")"

CC="${CC:-${TARGET}-gcc}"
CXX="${CXX:-${TARGET}-g++}"
STRIP="${STRIP:-${TARGET}-strip}"

cleanup() {
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

for tool in "$CC" "$CXX" "$STRIP" make; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "ERROR: required build tool is unavailable: $tool" >&2
        exit 1
    fi
done

if [[ ! -f "$CROSS_PREFIX/include/curl/curl.h" ]]; then
    echo "ERROR: Windows libcurl headers were not found in $CROSS_PREFIX" >&2
    exit 1
fi

rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/bin"

copy_source_tree() {
    local destination="$1"

    mkdir -p "$destination"
    tar \
        --exclude='.git' \
        --exclude='.deps' \
        --exclude='autom4te.cache' \
        --exclude='build' \
        --exclude='release' \
        --exclude='*.o' \
        --exclude='*.lo' \
        --exclude='*.a' \
        --exclude='*.exe' \
        -C "$ROOT_DIR" -cf - . | tar -C "$destination" -xf -
}

build_target() {
    local binary_name="$1"
    local cflags="$2"
    local target_source="$WORK_DIR/$binary_name"

    echo
    echo "Building ${binary_name}.exe"
    echo "CFLAGS=${cflags}"

    copy_source_tree "$target_source"
    cd "$target_source"

    CC="$CC" \
    CXX="$CXX" \
    CFLAGS="$cflags" \
    CPPFLAGS="-I${CROSS_PREFIX}/include" \
    LDFLAGS="-L${CROSS_PREFIX}/lib" \
        ./configure \
            --host="$TARGET" \
            --with-curl="$CROSS_PREFIX"

    make clean
    make -j"$JOBS"
    "$STRIP" -s cpuminer.exe
    install -m 0755 cpuminer.exe "$OUTPUT_DIR/bin/${binary_name}.exe"

    if ! strings "$OUTPUT_DIR/bin/${binary_name}.exe" | grep -x 'civiclight' >/dev/null; then
        echo "ERROR: ${binary_name}.exe was built without CivicLight support" >&2
        exit 1
    fi

    cd "$ROOT_DIR"
}

copy_runtime_dll() {
    local dll_name="$1"
    local dll_path=""
    local search_root

    for search_root in \
        "$CROSS_PREFIX/bin" \
        "/usr/${TARGET}/bin" \
        "/usr/${TARGET}/lib" \
        "/usr/lib/gcc/${TARGET}"; do
        [[ -d "$search_root" ]] || continue
        dll_path="$(find "$search_root" -type f -name "$dll_name" -print -quit)"
        [[ -n "$dll_path" ]] && break
    done

    if [[ -z "$dll_path" ]]; then
        echo "ERROR: required Windows runtime DLL was not found: $dll_name" >&2
        exit 1
    fi

    install -m 0644 "$dll_path" "$OUTPUT_DIR/bin/$dll_name"
}

echo "Preparing CivicLight multi-CPU Windows build"
if [[ "${SKIP_AUTOGEN:-0}" != "1" ]]; then
    ./autogen.sh
fi

# The launcher itself is intentionally compiled for the oldest supported CPU.
"$CC" \
    -O2 -march=core2 -Wall -Wextra -municode \
    -static-libgcc \
    "$ROOT_DIR/windows/civiclight-launcher.c" \
    -o "$OUTPUT_DIR/civiclight-miner.exe"
"$STRIP" -s "$OUTPUT_DIR/civiclight-miner.exe"

# Keep feature flags explicit so the launcher can test every required feature.
build_target "civiclight-core2" \
    "-O3 -march=core2 -Wall"

build_target "civiclight-avx" \
    "-O3 -mavx -maes -msse4.1 -msse4.2 -mpopcnt -Wall"

build_target "civiclight-avx2" \
    "-O3 -mavx2 -maes -msse4.1 -msse4.2 -mpopcnt -mbmi -mbmi2 -mfma -mmovbe -Wall"

build_target "civiclight-avx2-sha" \
    "-O3 -mavx2 -maes -msha -msse4.1 -msse4.2 -mpopcnt -mbmi -mbmi2 -mfma -mmovbe -Wall"

build_target "civiclight-avx512" \
    "-O3 -mavx2 -maes -msse4.1 -msse4.2 -mpopcnt -mbmi -mbmi2 -mfma -mmovbe -mavx512f -mavx512dq -mavx512cd -mavx512bw -mavx512vl -Wall"

build_target "civiclight-avx512-sha" \
    "-O3 -mavx2 -maes -msha -msse4.1 -msse4.2 -mpopcnt -mbmi -mbmi2 -mfma -mmovbe -mavx512f -mavx512dq -mavx512cd -mavx512bw -mavx512vl -Wall"

copy_runtime_dll "libcurl-4.dll"
copy_runtime_dll "zlib1.dll"
copy_runtime_dll "libwinpthread-1.dll"
copy_runtime_dll "libstdc++-6.dll"
copy_runtime_dll "libgcc_s_seh-1.dll"

echo
echo "Built CivicLight Windows files in: $OUTPUT_DIR"
find "$OUTPUT_DIR" -maxdepth 2 -type f -printf '%P\n' | sort
