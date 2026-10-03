#!/bin/sh
# Optional GitHub-only automation using GitHub CLI (gh).
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT"
. ./GIT_RELEASE.env

usage() {
  echo "Usage: ./github_publish.sh [OWNER/REPO] [public|private]" >&2
  echo "Default repository: $TOYOS_GITHUB_REPO" >&2
  exit 2
}
REPO=${1:-$TOYOS_GITHUB_REPO}
VIS=${2:-public}
case "$VIS" in public|private) ;; *) usage ;; esac

command -v gh >/dev/null 2>&1 || { echo "ERROR: GitHub CLI 'gh' is not installed." >&2; exit 1; }
gh auth status >/dev/null 2>&1 || { echo "ERROR: run 'gh auth login' first." >&2; exit 1; }

./git_publish.sh --local

REMOTE_URL="https://github.com/$REPO.git"
if gh repo view "$REPO" >/dev/null 2>&1; then
    echo "GitHub repository already exists: $REPO"
else
    gh repo create "$REPO" "--$VIS" --description "ToyOS v67.11-B FIX60ZEI stable baseline"
fi

./git_publish.sh "$REMOTE_URL"

mkdir -p dist
ARCHIVE="dist/ToyOS_v67_11_B_FIX60ZEI_SOURCE.zip"
git archive --format=zip --output="$ARCHIVE" "$TOYOS_GIT_TAG"

if gh release view "$TOYOS_GIT_TAG" --repo "$REPO" >/dev/null 2>&1; then
    echo "GitHub Release already exists: $TOYOS_GIT_TAG"
else
    gh release create "$TOYOS_GIT_TAG" "$ARCHIVE" \
      --repo "$REPO" \
      --title "$TOYOS_RELEASE_TITLE" \
      --notes-file GITHUB_RELEASE_NOTES_FIX60ZEI_RU.md
fi

echo "GitHub publication/release completed: https://github.com/$REPO"
