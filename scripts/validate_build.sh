#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_ROOT="${ROOT_DIR}/build"
DEBUG_DIR="${BUILD_ROOT}/debug"
RELEASE_DIR="${BUILD_ROOT}/release"
VCPKG_ROOT="${HOME}/vcpkg"

RUN_SANDBOX=1
CLEAN=1
USE_VCPKG=auto

usage() {
  cat <<'EOF'
Usage: scripts/validate_build.sh [options]

Options:
  --no-clean     Keep existing build folders
  --skip-run     Build only (do not run sandbox)
  --with-vcpkg   Force CMake toolchain from ~/vcpkg
  --no-vcpkg     Do not use vcpkg toolchain
  -h, --help     Show this help
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --no-clean)
      CLEAN=0
      shift
      ;;
    --skip-run)
      RUN_SANDBOX=0
      shift
      ;;
    --with-vcpkg)
      USE_VCPKG=on
      shift
      ;;
    --no-vcpkg)
      USE_VCPKG=off
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      usage
      exit 1
      ;;
  esac
done

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

  if [[ -z "${SDL_VIDEODRIVER:-}" ]]; then
    if [[ -S "/mnt/wslg/runtime-dir/wayland-0" ]] || [[ -n "${WAYLAND_DISPLAY:-}" && -S "${XDG_RUNTIME_DIR:-}/$WAYLAND_DISPLAY" ]]; then
      export SDL_VIDEODRIVER="wayland"
    else
      export SDL_VIDEODRIVER="x11"
    fi
  fi

  if [[ -z "${BGFX_RENDERER_TYPE:-}" ]]; then
    if is_wsl; then
      export BGFX_RENDERER_TYPE="noop"
    else
      export BGFX_RENDERER_TYPE="opengl"
    fi
  fi

  log "Runtime video setup: SDL_VIDEODRIVER=${SDL_VIDEODRIVER} BGFX_RENDERER_TYPE=${BGFX_RENDERER_TYPE} DISPLAY=${DISPLAY} WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-<unset>}"
}

cmake_configure() {
  local config_dir="$1"
  local build_type="$2"
  local args=(-S "$ROOT_DIR" -B "$config_dir" "-DCMAKE_BUILD_TYPE=${build_type}")

  if [[ "$USE_VCPKG" == "on" ]] || [[ "$USE_VCPKG" == "auto" && -f "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" ]]; then
    args+=("-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    log "Using vcpkg toolchain from ${VCPKG_ROOT}"
  fi

  cmake "${args[@]}"
}

find_sandbox_binary() {
  local build_dir="$1"
  local config="$2"
  local candidates=(
    "${build_dir}/apps/sandbox/sandbox"
    "${build_dir}/apps/sandbox/sandbox.exe"
    "${build_dir}/apps/sandbox/${config}/sandbox"
    "${build_dir}/apps/sandbox/${config}/sandbox.exe"
  )

  local bin
  for bin in "${candidates[@]}"; do
    if [[ -f "$bin" ]]; then
      echo "$bin"
      return 0
    fi
  done

  return 1
}

if [[ $CLEAN -eq 1 ]]; then
  log "Cleaning build directories"
  rm -rf "$DEBUG_DIR" "$RELEASE_DIR"
fi

log "Configuring Debug"
cmake_configure "$DEBUG_DIR" "Debug"

log "Building Debug"
cmake --build "$DEBUG_DIR" --config Debug

log "Configuring Release"
cmake_configure "$RELEASE_DIR" "Release"

log "Building Release"
cmake --build "$RELEASE_DIR" --config Release

if [[ $RUN_SANDBOX -eq 1 ]]; then
  setup_graphics_env

  log "Running sandbox (Debug)"
  DEBUG_BIN="$(find_sandbox_binary "$DEBUG_DIR" "Debug")"
  "$DEBUG_BIN"

  log "Running sandbox (Release)"
  RELEASE_BIN="$(find_sandbox_binary "$RELEASE_DIR" "Release")"
  "$RELEASE_BIN"
fi

log "Build validation completed successfully"
