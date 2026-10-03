#!/bin/sh
# ToyOS FIX60ZEI - publish the exact frozen tree on top of an existing GitHub repo.
# SSH-only GitHub publication path. No HTTPS browser login / GCM dependency.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
REPO_URL=${TOYOS_REPO_URL:-git@github.com:AB-bot1968/ToyOS.git}
TAG=v67.11-B-FIX60ZEI
AUTHOR='Ботнев Александр Валерьевич'
EMAIL='101033309+AB-bot1968@users.noreply.github.com'
AUTH_LOG="$ROOT/GITHUB_SSH_LAST_ERROR.log"

say() { printf '%s\n' "$*"; }
fail() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }
cleanup() {
    [ -z "${WORK:-}" ] || rm -rf "$WORK"
}
trap cleanup EXIT HUP INT TERM

command -v git >/dev/null 2>&1 || fail "git is required"
cd "$ROOT"
sh ./git_preflight.sh

# GitHub publication uses SSH only.  For local/offline regression tests,
# TOYOS_REPO_URL may point to a local/bare repository and this SSH block is skipped.
case "$REPO_URL" in
    git@github.com:*|ssh://git@github.com/*)
        # W64DevKit is not MSYS2: /c/Program Files/... may not exist even when
        # Git for Windows and its OpenSSH are installed. Prefer PATH, then
        # derive the Git-for-Windows root from git --exec-path. Do not validate
        # a native Windows path with POSIX [ -x ]; git ls-remote below is the
        # authoritative executable/authentication test.
        SSH_BIN=${TOYOS_SSH_BIN:-}
        if [ -n "$SSH_BIN" ]; then
            : # explicit override, useful for unusual W64DevKit installations
        elif command -v ssh >/dev/null 2>&1; then
            SSH_BIN=$(command -v ssh)
        else
            GIT_EXEC_PATH=$(git --exec-path 2>/dev/null | tr -d '\r' || true)
            case "$GIT_EXEC_PATH" in
                */mingw64/libexec/git-core)
                    GIT_ROOT=${GIT_EXEC_PATH%/mingw64/libexec/git-core}
                    SSH_BIN="$GIT_ROOT/usr/bin/ssh.exe"
                    ;;
                */mingw32/libexec/git-core)
                    GIT_ROOT=${GIT_EXEC_PATH%/mingw32/libexec/git-core}
                    SSH_BIN="$GIT_ROOT/usr/bin/ssh.exe"
                    ;;
                *)
                    # Last-resort default for Git for Windows. Forward slashes
                    # are intentional and accepted by Git for Windows.
                    SSH_BIN='C:/Program Files/Git/usr/bin/ssh.exe'
                    ;;
            esac
        fi

        # Quote the executable inside GIT_SSH_COMMAND so paths containing
        # "Program Files" work from W64DevKit's shell.
        GIT_SSH_COMMAND="\"$SSH_BIN\""
        export GIT_SSH_COMMAND
        unset GIT_SSH 2>/dev/null || true
        export GIT_SSH_VARIANT=ssh
        say "SSH: Git for Windows candidate: $SSH_BIN"
        ;;
esac

# Read/authentication test before any release work.  git ls-remote returns 0 on
# successful GitHub SSH authentication, unlike 'ssh -T', which intentionally
# returns a non-shell status even after successful authentication.
say "SSH: checking repository and authentication..."
if ! git ls-remote "$REPO_URL" HEAD >/dev/null 2>"$AUTH_LOG"; then
    say "SSH ERROR: cannot read repository $REPO_URL" >&2
    say "No release commit/tag has been created or pushed by this run." >&2
    say "Diagnostic log: $AUTH_LOG" >&2
    say "Manual check:" >&2
    say "  \"C:\\Program Files\\Git\\usr\\bin\\ssh.exe\" -T git@github.com" >&2
    exit 20
fi
say "SSH OK: repository authentication confirmed."
rm -f "$AUTH_LOG"

# Clone only the current bootstrap first. Then confirm WRITE access with a
# no-op dry-run push before copying/committing the 1100+ frozen files.
WORK=${TMPDIR:-/tmp}/toyos_fix60zei_publish_$$
rm -rf "$WORK"
say "SSH: cloning current main..."
if ! git clone --quiet "$REPO_URL" "$WORK" 2>"$AUTH_LOG"; then
    say "SSH ERROR: git clone failed. See $AUTH_LOG" >&2
    exit 21
fi

cd "$WORK"
git config user.name "$AUTHOR"
git config user.email "$EMAIL"

say "SSH: testing write access (dry-run; remote is not changed)..."
if ! git push --dry-run origin HEAD:main >/dev/null 2>"$AUTH_LOG"; then
    say "SSH ERROR: write access test failed." >&2
    say "No ToyOS release commit/tag has been created or pushed by this run." >&2
    say "Diagnostic log: $AUTH_LOG" >&2
    exit 22
fi
say "SSH OK: write access confirmed."
rm -f "$AUTH_LOG"

# Copy the exact publication tree without touching destination .git.
say "PUBLISH: copying FIX60ZEI publication tree..."
( cd "$ROOT" && tar \
    --exclude='./.git' \
    --exclude='./build' \
    --exclude='./dist' \
    --exclude='./GITHUB_AUTH_LAST_ERROR.log' \
    --exclude='./GITHUB_SSH_LAST_ERROR.log' \
    -cf - . ) | ( cd "$WORK" && tar -xf - )
cd "$WORK"

# Re-run the frozen baseline verification inside the actual clone to be pushed.
sh ./git_preflight.sh

git add -A
if git diff --cached --quiet; then
    say "PUBLISH: repository content already matches FIX60ZEI; no content commit needed."
else
    git commit -m "Publish ToyOS v67.11-B FIX60ZEI stable baseline"
fi

HEAD_SHA=$(git rev-parse HEAD)
if git rev-parse "$TAG" >/dev/null 2>&1; then
    TAG_SHA=$(git rev-list -n1 "$TAG")
    [ "$TAG_SHA" = "$HEAD_SHA" ] || fail "tag $TAG exists on another commit ($TAG_SHA)"
    say "PUBLISH: tag $TAG already points to current commit."
else
    git tag -a "$TAG" -m "ToyOS v67.11-B FIX60ZEI stable baseline"
fi

# Never force. A concurrent remote update must stop publication for review.
say "PUBLISH: pushing main (fast-forward only)..."
if ! git push origin HEAD:main 2>"$AUTH_LOG"; then
    say "PUSH ERROR: main was not published. No force push was attempted." >&2
    say "Diagnostic log: $AUTH_LOG" >&2
    exit 30
fi

say "PUBLISH: pushing tag $TAG..."
if ! git push origin "refs/tags/$TAG" 2>"$AUTH_LOG"; then
    say "TAG PUSH ERROR: main was published, but tag push failed." >&2
    say "Run this script again after fixing SSH/network; it is idempotent." >&2
    say "Diagnostic log: $AUTH_LOG" >&2
    exit 31
fi
rm -f "$AUTH_LOG"

say "PUBLISHED: $REPO_URL"
say "COMMIT: $HEAD_SHA"
say "TAG: $TAG"
