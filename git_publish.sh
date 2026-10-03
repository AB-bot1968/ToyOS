#!/bin/sh
# ToyOS FIX60ZEI: safe, idempotent initial Git publication.
# No force push. Existing unrelated Git history is rejected.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT"
. ./GIT_RELEASE.env

usage() {
    cat <<USAGE
Usage:
  ./git_publish.sh --local
  ./git_publish.sh <git-remote-url>
  ./git_publish.sh --dry-run [git-remote-url]

Examples:
  ./git_publish.sh --local
  ./git_publish.sh https://github.com/USER/ToyOS.git
  ./git_publish.sh git@github.com:USER/ToyOS.git
USAGE
}

MODE=publish
REMOTE_URL=
case "${1:-}" in
    -h|--help) usage; exit 0 ;;
    --local) MODE=local; [ "$#" -eq 1 ] || { usage >&2; exit 2; } ;;
    --dry-run)
        MODE=dry
        REMOTE_URL=${2:-}
        [ "$#" -le 2 ] || { usage >&2; exit 2; }
        ;;
    '') usage >&2; exit 2 ;;
    *) REMOTE_URL=$1; [ "$#" -eq 1 ] || { usage >&2; exit 2; } ;;
esac

./git_preflight.sh

command -v git >/dev/null 2>&1 || { echo "ERROR: git is not available in PATH." >&2; exit 1; }

# Initialize first so author identity can be stored only in this repository.
if [ ! -d .git ]; then
    git init
fi

# Publication identity is fixed locally for this release. It does not modify
# the user's global Git configuration and uses GitHub's noreply address.
git config user.name "$TOYOS_AUTHOR_NAME"
git config user.email "$TOYOS_AUTHOR_EMAIL"
NAME=$(git config --get user.name)
EMAIL=$(git config --get user.email)
[ "$NAME" = "$TOYOS_AUTHOR_NAME" ] || { echo "ERROR: Git author name mismatch." >&2; exit 1; }
[ "$EMAIL" = "$TOYOS_AUTHOR_EMAIL" ] || { echo "ERROR: Git author email mismatch." >&2; exit 1; }

if [ "$MODE" = dry ]; then
    echo "DRY RUN OK"
    echo "  release : $TOYOS_RELEASE_NAME"
    echo "  branch  : $TOYOS_GIT_BRANCH"
    echo "  tag     : $TOYOS_GIT_TAG"
    echo "  commit  : $TOYOS_COMMIT_MESSAGE"
    [ -z "$REMOTE_URL" ] || echo "  remote  : $REMOTE_URL"
    exit 0
fi

# A publish-ready archive is intended to create one root release commit.
# Re-running the script is allowed only for the exact release already prepared.
if git rev-parse --verify HEAD >/dev/null 2>&1; then
    HEAD_SHA=$(git rev-parse HEAD)
    TAG_SHA=$(git rev-list -n 1 "$TOYOS_GIT_TAG" 2>/dev/null || true)
    if [ -n "$TAG_SHA" ] && [ "$HEAD_SHA" = "$TAG_SHA" ] && \
       git diff --quiet && git diff --cached --quiet; then
        echo "Local release commit/tag already prepared: $HEAD_SHA"
    else
        echo "ERROR: this directory already contains unrelated or modified Git history." >&2
        echo "Use a fresh publish-ready FIX60ZEI tree, or inspect the repository manually." >&2
        exit 1
    fi
else
    git add -A

    # The frozen historical tree intentionally contains legacy whitespace in old
    # patch/history files. Do not rewrite stable baseline content merely to satisfy
    # Git whitespace heuristics. Check only publication-layer files added/edited now.
    git diff --cached --check -- \
      .gitignore README.md GIT_SETUP_RU.md GIT_RELEASE.env \
      git_preflight.sh git_preflight.bat git_publish.sh git_publish.bat \
      github_publish.sh github_publish.bat LICENSE_STATUS_RU.md COPYRIGHT AUTHORS.md \
      GITHUB_RELEASE_NOTES_FIX60ZEI_RU.md FIX60ZEI_FROZEN_CONTENT_SHA256.txt \
      TOYOS_ARCHITECTURE_VISION.md .github/workflows/preflight.yml

    # Required resources must really be staged despite generic *.raw/*.bin ignores.
    git ls-files --error-unmatch resources/SPLASH.RAW >/dev/null 2>&1 || {
        echo "ERROR: resources/SPLASH.RAW is not staged/tracked." >&2; exit 1; }
    git ls-files --error-unmatch tools/Python/TEST.BIN >/dev/null 2>&1 || {
        echo "ERROR: tools/Python/TEST.BIN is not staged/tracked." >&2; exit 1; }

    git diff --cached --quiet && { echo "ERROR: nothing to commit." >&2; exit 1; }
    STAGED_COUNT=$(git diff --cached --name-only | wc -l | tr -d ' ')
    echo "Staged files: $STAGED_COUNT"
    echo "Publication-layer changes:"
    git status --short -- \
      .gitignore README.md GIT_SETUP_RU.md GIT_RELEASE.env \
      git_preflight.sh git_preflight.bat git_publish.sh git_publish.bat \
      github_publish.sh github_publish.bat LICENSE_STATUS_RU.md COPYRIGHT AUTHORS.md \
      GITHUB_RELEASE_NOTES_FIX60ZEI_RU.md FIX60ZEI_FROZEN_CONTENT_SHA256.txt \
      TOYOS_ARCHITECTURE_VISION.md .github/workflows/preflight.yml
    git commit -q -m "$TOYOS_COMMIT_MESSAGE"
    git branch -M "$TOYOS_GIT_BRANCH"
    git tag -a "$TOYOS_GIT_TAG" -m "$TOYOS_RELEASE_TITLE"
fi

if [ "$MODE" = local ]; then
    echo "Local Git release prepared. No remote was changed."
    echo "Branch: $TOYOS_GIT_BRANCH"
    echo "Tag:    $TOYOS_GIT_TAG"
    exit 0
fi

if [ -z "$REMOTE_URL" ]; then
    echo "ERROR: remote URL is required for publication." >&2
    exit 2
fi

if git remote get-url origin >/dev/null 2>&1; then
    CURRENT=$(git remote get-url origin)
    [ "$CURRENT" = "$REMOTE_URL" ] || {
        echo "ERROR: origin already points to a different URL: $CURRENT" >&2
        exit 1
    }
else
    git remote add origin "$REMOTE_URL"
fi

LOCAL_SHA=$(git rev-parse HEAD)
REMOTE_MAIN=$(git ls-remote origin "refs/heads/$TOYOS_GIT_BRANCH" 2>/dev/null | awk 'NR==1{print $1}')
if [ -n "$REMOTE_MAIN" ]; then
    [ "$REMOTE_MAIN" = "$LOCAL_SHA" ] || {
        echo "ERROR: remote $TOYOS_GIT_BRANCH already exists with different history." >&2
        echo "No force push will be attempted." >&2
        exit 1
    }
    echo "Remote branch already matches local release commit."
else
    git push -u origin "$TOYOS_GIT_BRANCH"
fi

LOCAL_TAG_SHA=$(git rev-list -n 1 "$TOYOS_GIT_TAG")
REMOTE_TAG_SHA=$(git ls-remote origin "refs/tags/$TOYOS_GIT_TAG^{}" 2>/dev/null | awk 'NR==1{print $1}')
if [ -z "$REMOTE_TAG_SHA" ]; then
    # For lightweight/servers that do not advertise ^{}, also check tag object existence.
    REMOTE_TAG_OBJ=$(git ls-remote origin "refs/tags/$TOYOS_GIT_TAG" 2>/dev/null | awk 'NR==1{print $1}')
    if [ -n "$REMOTE_TAG_OBJ" ]; then
        echo "Remote tag exists; verifying via fetch."
        git fetch -q origin "refs/tags/$TOYOS_GIT_TAG:refs/tags/__toyos_remote_verify" || true
        VERIFY_SHA=$(git rev-list -n 1 refs/tags/__toyos_remote_verify 2>/dev/null || true)
        git tag -d __toyos_remote_verify >/dev/null 2>&1 || true
        [ "$VERIFY_SHA" = "$LOCAL_TAG_SHA" ] || { echo "ERROR: remote tag differs." >&2; exit 1; }
    else
        git push origin "$TOYOS_GIT_TAG"
    fi
else
    [ "$REMOTE_TAG_SHA" = "$LOCAL_TAG_SHA" ] || { echo "ERROR: remote annotated tag differs." >&2; exit 1; }
    echo "Remote tag already matches local release."
fi

echo "Git publication completed safely."
echo "  release: $TOYOS_RELEASE_NAME"
echo "  commit : $LOCAL_SHA"
echo "  branch : $TOYOS_GIT_BRANCH"
echo "  tag    : $TOYOS_GIT_TAG"
