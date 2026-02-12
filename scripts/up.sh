#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/debug"
VCPKG_TOOLCHAIN="${HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake"

log() {
  printf "\n==> %s\n" "$1"
}

CONFIGURE_ARGS=(
  -S "$ROOT_DIR"
  -B "$BUILD_DIR"
  -DCMAKE_BUILD_TYPE=Debug
)

if [[ -f "$VCPKG_TOOLCHAIN" ]]; then
  CONFIGURE_ARGS+=("-DCMAKE_TOOLCHAIN_FILE=${VCPKG_TOOLCHAIN}")
  log "Using vcpkg toolchain: ${VCPKG_TOOLCHAIN}"
else
  log "vcpkg toolchain not found, running without it"
fi

log "Configuring Debug"
cmake "${CONFIGURE_ARGS[@]}"

log "Building Debug"
cmake --build "$BUILD_DIR" --config Debug

SANDBOX_BIN="${BUILD_DIR}/apps/sandbox/sandbox"
if [[ ! -f "$SANDBOX_BIN" ]]; then
  echo "Sandbox binary not found at ${SANDBOX_BIN}" >&2
  exit 1
fi

log "Running sandbox"
"$SANDBOX_BIN"
