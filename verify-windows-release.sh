#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGE_DIR="${1:-${ROOT_DIR}/build/civiclight-miner-windows-x86_64}"

required_files=(
    "civiclight-miner.exe"
    "start-mining.bat"
    "README.md"
    "RELEASE_NOTES.md"
    "LICENSE"
    "COPYING"
    "bin/civiclight-core2.exe"
    "bin/civiclight-avx.exe"
    "bin/civiclight-avx2.exe"
    "bin/civiclight-avx2-sha.exe"
    "bin/civiclight-avx512.exe"
    "bin/civiclight-avx512-sha.exe"
    "bin/libcurl-4.dll"
    "bin/zlib1.dll"
    "bin/libwinpthread-1.dll"
    "bin/libstdc++-6.dll"
    "bin/libgcc_s_seh-1.dll"
)

for relative_path in "${required_files[@]}"; do
    if [[ ! -f "$PACKAGE_DIR/$relative_path" ]]; then
        echo "ERROR: Windows release file is missing: $relative_path" >&2
        exit 1
    fi
done

if ! file "$PACKAGE_DIR/civiclight-miner.exe" | grep -q 'PE32+ executable.*x86-64'; then
    echo "ERROR: launcher is not a Windows x86-64 executable" >&2
    exit 1
fi

for miner in "$PACKAGE_DIR"/bin/civiclight-*.exe; do
    if ! file "$miner" | grep -q 'PE32+ executable.*x86-64'; then
        echo "ERROR: miner is not a Windows x86-64 executable: $miner" >&2
        exit 1
    fi
    if ! strings "$miner" | grep -x 'civiclight' >/dev/null; then
        echo "ERROR: miner does not contain CivicLight algorithm registration: $miner" >&2
        exit 1
    fi
done

if ! strings -el "$PACKAGE_DIR/civiclight-miner.exe" | grep -q 'CivicLight Windows CPU Miner'; then
    echo "ERROR: launcher identification string is missing" >&2
    exit 1
fi

echo "Windows release validation passed: $PACKAGE_DIR"
