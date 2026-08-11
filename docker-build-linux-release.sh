#!/usr/bin/env bash

set -Eeuo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUTPUT_DIR="${1:-${ROOT_DIR}/release}"
IMAGE_NAME="${IMAGE_NAME:-civiclight-miner-linux-builder:ubuntu22}"
container_id=""

cleanup() {
    if [[ -n "$container_id" ]]; then
        docker rm -f "$container_id" >/dev/null 2>&1 || true
    fi
}
trap cleanup EXIT

mkdir -p "$OUTPUT_DIR"

docker build \
    -f "$ROOT_DIR/Dockerfile.linux-release" \
    -t "$IMAGE_NAME" \
    "$ROOT_DIR"

container_id="$(docker create "$IMAGE_NAME")"
docker cp "$container_id:/release/." "$OUTPUT_DIR/"

echo
echo "Copied Linux release artifacts to: $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"
