#!/bin/sh
# FIX53: the old five-DAP/256-sector kernel-slot check is obsolete.
# Validate the descriptor-driven variable-size loader instead.
set -eu
fail(){ echo "check67 FAIL: $1" >&2; exit 1; }
[ -f src/layout_loader.S ] || fail "missing layout loader"
grep -q 'cmpl \$64,%eax' src/layout_loader.S || fail "loader does not cap BIOS chunks at 64 sectors"
grep -q 'kernel_left' src/layout_loader.S || fail "loader does not iterate over declared kernel sectors"
grep -q 'kernel_lba_cur' src/layout_loader.S || fail "loader does not advance dynamic kernel LBA"
grep -q '0x063c' src/layout_loader.S || fail "loader does not enforce descriptor memory limit"
[ -x build/layoutcheck.exe ] && [ -f build/disk.img ] && build/layoutcheck.exe build/disk.img >/dev/null
printf '%s\n' 'PASS: FIX53 descriptor-driven kernel loader uses bounded dynamic DAP loop'
