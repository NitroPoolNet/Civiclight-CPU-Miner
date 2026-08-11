#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RELEASE_DIR="${1:-${ROOT_DIR}/release}"
PACKAGE_NAME="civiclight-miner-linux-x86_64"
STAGE_DIR="${ROOT_DIR}/build/${PACKAGE_NAME}"

rm -rf "$STAGE_DIR"
mkdir -p "$STAGE_DIR/bin" "$RELEASE_DIR"

"$ROOT_DIR/nitro-build-allarch.sh" "$STAGE_DIR/bin"

install -m 0755 "$ROOT_DIR/civiclight-miner" "$STAGE_DIR/civiclight-miner"
install -m 0644 "$ROOT_DIR/LINUX_RELEASE_README.md" "$STAGE_DIR/README.md"
install -m 0644 "$ROOT_DIR/COPYING" "$STAGE_DIR/COPYING"
install -m 0644 "$ROOT_DIR/LICENSE" "$STAGE_DIR/LICENSE"

tar -C "$ROOT_DIR/build" -czf "$RELEASE_DIR/${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"

echo
echo "Release artifact:"
ls -lh "$RELEASE_DIR/${PACKAGE_NAME}.tar.gz"
