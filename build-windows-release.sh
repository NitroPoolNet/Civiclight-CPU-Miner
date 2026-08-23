#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RELEASE_DIR="${1:-${ROOT_DIR}/release}"
PACKAGE_NAME="civiclight-miner-windows-x86_64"
STAGE_DIR="${ROOT_DIR}/build/${PACKAGE_NAME}"

rm -rf "$STAGE_DIR"
mkdir -p "$RELEASE_DIR"

"$ROOT_DIR/nitro-build-windows-allarch.sh" "$STAGE_DIR"

install -m 0644 "$ROOT_DIR/WINDOWS_RELEASE_README.md" "$STAGE_DIR/README.md"
install -m 0644 "$ROOT_DIR/WINDOWS_RELEASE_NOTES.md" "$STAGE_DIR/RELEASE_NOTES.md"
install -m 0644 "$ROOT_DIR/COPYING" "$STAGE_DIR/COPYING"
install -m 0644 "$ROOT_DIR/LICENSE" "$STAGE_DIR/LICENSE"
install -m 0644 "$ROOT_DIR/windows/start-mining.bat" "$STAGE_DIR/start-mining.bat"

"$ROOT_DIR/verify-windows-release.sh" "$STAGE_DIR"

rm -f "$RELEASE_DIR/${PACKAGE_NAME}.zip"
(
    cd "$ROOT_DIR/build"
    zip -q -r "$RELEASE_DIR/${PACKAGE_NAME}.zip" "$PACKAGE_NAME"
)

echo
echo "Windows release artifact:"
ls -lh "$RELEASE_DIR/${PACKAGE_NAME}.zip"
