#!/usr/bin/env bash
set -euo pipefail

VCPKG_ROOT="${HOME}/vcpkg"
TRIPLET="x64-linux"
RUN_APT=1

usage() {
  cat <<'EOF'
Usage: scripts/install_external_deps.sh [options]

Options:
  --vcpkg-root <path>   Custom vcpkg directory (default: ~/vcpkg)
  --triplet <name>      vcpkg triplet (default: x64-linux)
  --skip-apt            Do not install apt packages
  -h, --help            Show this help
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --vcpkg-root)
      VCPKG_ROOT="$2"
      shift 2
      ;;
    --triplet)
      TRIPLET="$2"
      shift 2
      ;;
    --skip-apt)
      RUN_APT=0
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
done

log() {
  printf "\n==> %s\n" "$1"
}

is_vcpkg_pkg_installed() {
  local pkg="$1"
  local pattern="^${pkg}:${TRIPLET}[[:space:]]"
  "${VCPKG_ROOT}/vcpkg" list | grep -Eq "${pattern}"
}

require_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "Missing required command: $1" >&2
    exit 1
  fi
}

if [[ $RUN_APT -eq 1 ]]; then
  require_cmd sudo
  require_cmd apt
  APT_PACKAGES=(
    git
    curl
    zip
    unzip
    tar
    pkg-config
    ninja-build
    autoconf
    autoconf-archive
    automake
    libtool
    python3-venv
  )
  MISSING_APT_PACKAGES=()

  for pkg in "${APT_PACKAGES[@]}"; do
    if ! dpkg -s "${pkg}" >/dev/null 2>&1; then
      MISSING_APT_PACKAGES+=("${pkg}")
    fi
  done

  if [[ ${#MISSING_APT_PACKAGES[@]} -eq 0 ]]; then
    log "System dependencies already installed"
  else
    log "Installing missing system dependencies via apt"
    sudo apt update
    sudo apt install -y "${MISSING_APT_PACKAGES[@]}"
  fi
fi

require_cmd git
require_cmd bash

if [[ -d "${VCPKG_ROOT}/.git" ]]; then
  log "Updating existing vcpkg at ${VCPKG_ROOT}"
  git -C "${VCPKG_ROOT}" pull --ff-only
else
  log "Cloning vcpkg into ${VCPKG_ROOT}"
  git clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}"
fi

if [[ ! -x "${VCPKG_ROOT}/vcpkg" ]]; then
  log "Bootstrapping vcpkg"
  "${VCPKG_ROOT}/bootstrap-vcpkg.sh"
fi

SDL2_SPEC="sdl2[core,x11,wayland]"
BGFX_SPEC="bgfx"
FMT_SPEC="fmt"

MISSING_VCPKG_PKGS=()
if ! is_vcpkg_pkg_installed "sdl2"; then
  MISSING_VCPKG_PKGS+=("${SDL2_SPEC}")
fi
if ! is_vcpkg_pkg_installed "bgfx"; then
  MISSING_VCPKG_PKGS+=("${BGFX_SPEC}")
fi
if ! is_vcpkg_pkg_installed "fmt"; then
  MISSING_VCPKG_PKGS+=("${FMT_SPEC}")
fi

if [[ ${#MISSING_VCPKG_PKGS[@]} -eq 0 ]]; then
  log "vcpkg dependencies already installed for ${TRIPLET} (sdl2, bgfx, fmt)"
else
  log "Installing missing vcpkg dependencies (${TRIPLET})"
  "${VCPKG_ROOT}/vcpkg" install "${MISSING_VCPKG_PKGS[@]}" --triplet "${TRIPLET}"
fi

log "Done"
cat <<EOF
Dependencies installed.

Use this toolchain in CMake:
  -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake

Example:
  cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug \\
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
EOF
