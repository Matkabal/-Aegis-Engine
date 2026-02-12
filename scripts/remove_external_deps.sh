#!/usr/bin/env bash
set -euo pipefail

VCPKG_ROOT="${HOME}/vcpkg"
TRIPLET="x64-linux"
REMOVE_VCPKG_DIR=0

usage() {
  cat <<'EOF'
Usage: scripts/remove_external_deps.sh [options]

Options:
  --vcpkg-root <path>   Custom vcpkg directory (default: ~/vcpkg)
  --triplet <name>      vcpkg triplet (default: x64-linux)
  --remove-vcpkg-dir    Also remove the full vcpkg directory
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
    --remove-vcpkg-dir)
      REMOVE_VCPKG_DIR=1
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

if [[ ! -x "${VCPKG_ROOT}/vcpkg" ]]; then
  log "No vcpkg binary found at ${VCPKG_ROOT}; nothing to remove"
  exit 0
fi

log "Removing vcpkg dependencies (bgfx, sdl2, fmt) for ${TRIPLET}"
"${VCPKG_ROOT}/vcpkg" remove bgfx sdl2 fmt --triplet "${TRIPLET}" || true

if [[ $REMOVE_VCPKG_DIR -eq 1 ]]; then
  log "Removing vcpkg directory at ${VCPKG_ROOT}"
  rm -rf "${VCPKG_ROOT}"
fi

log "Done"
