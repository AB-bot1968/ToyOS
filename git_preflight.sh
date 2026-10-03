#!/bin/sh
# ToyOS FIX60ZEI: safe publication preflight. Does not modify Git history.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT"

fail() { echo "PREFLIGHT FAIL: $*" >&2; exit 1; }
warn() { echo "PREFLIGHT WARNING: $*" >&2; }
ok() { echo "PREFLIGHT PASS: $*"; }

[ -f GIT_RELEASE.env ] || fail "GIT_RELEASE.env is missing"
# shellcheck disable=SC1091
. ./GIT_RELEASE.env

[ -f VERSION.txt ] || fail "VERSION.txt is missing"
grep -q '^ToyOS v67\.11-B FIX60ZEI ' VERSION.txt || fail "unexpected VERSION.txt"
[ "$TOYOS_AUTHOR_NAME" = "Ботнев Александр Валерьевич" ] || fail "unexpected author name"
[ "$TOYOS_GITHUB_OWNER" = "AB-bot1968" ] || fail "unexpected GitHub owner"
[ "$TOYOS_GITHUB_REPO" = "AB-bot1968/ToyOS" ] || fail "unexpected GitHub repository"
[ -f COPYRIGHT ] || fail "COPYRIGHT is missing"
grep -Fqx 'Copyright © 2026 Ботнев Александр Валерьевич' COPYRIGHT || fail "copyright holder mismatch"
grep -Fqx 'All rights reserved.' COPYRIGHT || fail "copyright reservation missing"
ok "copyright holder verified"

command -v sha256sum >/dev/null 2>&1 || fail "sha256sum is required"
[ -f .gitattributes ] || fail ".gitattributes is missing"
grep -Fqx '* -text' .gitattributes || fail ".gitattributes must preserve exact bytes with * -text"
if grep -Eq 'eol=' .gitattributes; then fail ".gitattributes must not force EOL conversion"; fi
ok "Git attributes preserve exact frozen bytes"

[ -f FIX60ZEI_FROZEN_CONTENT_SHA256.txt ] || fail "frozen-content manifest is missing"
[ "$(wc -l < FIX60ZEI_FROZEN_CONTENT_SHA256.txt | tr -d ' ')" = "1109" ] || fail "expected 1109 frozen manifest entries"
sha256sum -c FIX60ZEI_FROZEN_CONTENT_SHA256.txt >/dev/null || fail "frozen FIX60ZEI content changed"
ok "frozen FIX60ZEI content matches manifest"

KERNEL_SHA=$(sha256sum src/kernel.c | awk '{print $1}')
[ "$KERNEL_SHA" = "$TOYOS_KERNEL_SHA256" ] || fail "src/kernel.c differs from stable baseline"
TESTMRT_SHA=$(sha256sum TST/TESTMRT.TST | awk '{print $1}')
[ "$TESTMRT_SHA" = "$TOYOS_TESTMRT_SHA256" ] || fail "TST/TESTMRT.TST differs from stable baseline"
ok "critical runtime/test hashes match FIX60ZEI"

[ ! -d build ] || fail "build/ must not be committed"
[ ! -d dist ] || fail "dist/ must not be committed"
[ ! -e TST.LOG ] || fail "TST.LOG is forbidden"
if find TST -maxdepth 1 -type f -name 'B*.TST' -print | grep -q .; then
    fail "B*.TST files are forbidden"
fi

[ -f resources/SPLASH.RAW ] || fail "resources/SPLASH.RAW is missing"
[ "$(wc -c < resources/SPLASH.RAW | tr -d ' ')" = "64000" ] || fail "resources/SPLASH.RAW must be 64000 bytes"
[ -f tools/Python/TEST.BIN ] || fail "tools/Python/TEST.BIN fixture is missing"
grep -qx '!resources/SPLASH.RAW' .gitignore || fail "SPLASH.RAW is not unignored"
grep -qx '!tools/Python/TEST.BIN' .gitignore || fail "TEST.BIN is not unignored"
ok "required binary resources are publication-safe"

COUNT=0
TMP_NAMES="${TMPDIR:-/tmp}/toyos_accept_$$.txt"
trap 'rm -f "$TMP_NAMES"' EXIT HUP INT TERM
: > "$TMP_NAMES"
while IFS= read -r raw || [ -n "$raw" ]; do
    name=$(printf '%s' "$raw" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
    [ -n "$name" ] || continue
    case "$name" in \#*) continue ;; esac
    [ -f "TST/$name" ] || fail "acceptance file missing: TST/$name"
    printf '%s\n' "$name" >> "$TMP_NAMES"
    COUNT=$((COUNT + 1))
done < TST/ACCEPT.TXT
[ "$COUNT" -eq 45 ] || fail "expected 45 acceptance tests, got $COUNT"
DUP=$(sort "$TMP_NAMES" | uniq -d | head -1 || true)
[ -z "$DUP" ] || fail "duplicate acceptance test: $DUP"
ok "45 unique acceptance tests are present"

# Existing release checker is stronger when Python 3 is available, but the
# publication preflight does not require Python on Windows/W64DevKit.
if command -v python3 >/dev/null 2>&1; then
    sh ./check_fix60zei_idle_heartbeat.sh >/dev/null || fail "FIX60ZEI checker failed"
    ok "FIX60ZEI source checker"
else
    warn "python3 not found; full check_fix60zei_idle_heartbeat.sh skipped"
fi

# Reject common accidentally published credentials. This is intentionally
# limited to strong token/key signatures to avoid false positives in docs.
BAD_SECRET_FILES=$(find . -path './.git' -prune -o -type f \( \
    -name '*.pem' -o -name '*.p12' -o -name '*.pfx' -o \
    -name 'id_rsa' -o -name 'id_rsa.*' -o -name 'id_ed25519' -o -name 'id_ed25519.*' \
    \) -print | head -1)
[ -z "$BAD_SECRET_FILES" ] || fail "possible private credential file: $BAD_SECRET_FILES"
if grep -IRnE --exclude-dir=.git --exclude='git_preflight.sh' \
    'github_pat_[A-Za-z0-9_]{20,}|ghp_[A-Za-z0-9]{20,}|-----BEGIN ([A-Z ]+ )?PRIVATE KEY-----|AKIA[0-9A-Z]{16}|xox[baprs]-[A-Za-z0-9-]{20,}' \
    . >/dev/null 2>&1; then
    fail "possible credential/token signature found in repository text"
fi
ok "no common credential signatures detected"

# BusyBox/W64DevKit find does not support GNU -size +50M. Use wc -c;
# 50 MiB = 52428800 bytes.
LARGE=$(find . -path './.git' -prune -o -type f -print | while IFS= read -r f; do
    bytes=$(wc -c < "$f" | tr -d '[:space:]')
    case "$bytes" in ''|*[!0-9]*) continue ;; esac
    if [ "$bytes" -gt 52428800 ]; then
        printf '%s\n' "$f"
        break
    fi
done)
[ -z "$LARGE" ] || fail "file larger than 50 MiB: $LARGE"
ok "no oversized publication files"

sh -n build.sh || fail "build.sh syntax error"
sh -n git_preflight.sh || fail "git_preflight.sh syntax error"
sh -n git_publish.sh || fail "git_publish.sh syntax error"
if [ -f github_publish.sh ]; then sh -n github_publish.sh || fail "github_publish.sh syntax error"; fi
ok "shell script syntax"

if command -v git >/dev/null 2>&1 && [ -d .git ]; then
    git diff --check || fail "git diff --check failed"
fi

if [ ! -f LICENSE ] && [ ! -f LICENSE.txt ] && [ ! -f LICENSE.md ]; then
    warn "LICENSE is not selected. Public publication is possible, but reuse rights are not explicitly granted. See LICENSE_STATUS_RU.md."
fi

echo "PREFLIGHT OK: $TOYOS_RELEASE_NAME is ready for Git commit/tag publication"
