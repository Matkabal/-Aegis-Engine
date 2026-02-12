#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REMOTE_URL="${1:-https://github.com/Matkabal/-Aegis-Engine.git}"
BRANCH="master"

log() {
  printf "\n==> %s\n" "$1"
}

cd "$ROOT_DIR"

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  log "Initializing git repository"
  git init
fi

log "Setting branch to ${BRANCH}"
git branch -M "$BRANCH"

log "Staging changes"
git add .

if git diff --cached --quiet; then
  log "No staged changes to commit"
else
  log "Creating commit"
  git commit -m "m0: foundation bootstrap with SDL2, bgfx, logging, timer, input and scripts"
fi

if git remote get-url origin >/dev/null 2>&1; then
  log "Updating existing origin remote"
  git remote set-url origin "$REMOTE_URL"
else
  log "Adding origin remote"
  git remote add origin "$REMOTE_URL"
fi

log "Pulling ${BRANCH} from origin (rebase)"
if ! git pull origin "$BRANCH" --rebase; then
  log "Rebase pull failed, retrying with unrelated histories"
  git pull origin "$BRANCH" --allow-unrelated-histories
fi

log "Pushing to origin/${BRANCH}"
git push -u origin "$BRANCH"

log "Publish completed"
