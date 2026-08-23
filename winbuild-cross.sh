#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "winbuild-cross.sh now uses the CivicLight reproducible Windows release build."
exec "$ROOT_DIR/docker-build-windows-release.sh" "$@"
