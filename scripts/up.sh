#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build/debug"
VCPKG_TOOLCHAIN="${HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake"

log() {
  printf "\n==> %s\n" "$1"
}

is_wsl() {
  grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null
}

setup_graphics_env() {
  if [[ -z "${XDG_RUNTIME_DIR:-}" && -d "/mnt/wslg/runtime-dir" ]]; then
    export XDG_RUNTIME_DIR="/mnt/wslg/runtime-dir"
  fi
  if [[ -z "${WAYLAND_DISPLAY:-}" && -S "${XDG_RUNTIME_DIR:-}/wayland-0" ]]; then
    export WAYLAND_DISPLAY="wayland-0"
  fi
  if [[ -z "${DISPLAY:-}" ]]; then
    export DISPLAY=":0"
  fi

  if [[ -n "${SDL_VIDEODRIVER:-}" ]]; then
    log "Using preset SDL_VIDEODRIVER=${SDL_VIDEODRIVER}"
  else
    if [[ -S "/mnt/wslg/runtime-dir/wayland-0" ]] || [[ -n "${WAYLAND_DISPLAY:-}" && -S "${XDG_RUNTIME_DIR:-}/$WAYLAND_DISPLAY" ]]; then
      export SDL_VIDEODRIVER="wayland"
    else
      export SDL_VIDEODRIVER="x11"
    fi
    log "Selected SDL_VIDEODRIVER=${SDL_VIDEODRIVER}"
  fi

  if [[ -z "${BGFX_RENDERER_TYPE:-}" ]]; then
    if is_wsl; then
      export BGFX_RENDERER_TYPE="noop"
    else
      export BGFX_RENDERER_TYPE="opengl"
    fi
    log "Selected BGFX_RENDERER_TYPE=${BGFX_RENDERER_TYPE}"
  else
    log "Using preset BGFX_RENDERER_TYPE=${BGFX_RENDERER_TYPE}"
  fi
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

setup_graphics_env

log "Running sandbox"
"$SANDBOX_BIN"
