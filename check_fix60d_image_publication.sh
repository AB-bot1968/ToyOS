#!/bin/sh
set -eu
fail(){ echo "FIX60D IMAGE PUBLICATION CHECK FAIL: $1" >&2; exit 1; }
grep -q '^# FIX60D: publish a verified bootable image immediately after disk assembly' build.sh || fail early-block
grep -q '^mv -f build/toy_os.img.tmp build/toy_os.img$' build.sh || fail early-atomic-publish
grep -q '^\./build/layoutcheck.exe build/toy_os.img.tmp$' build.sh || fail early-layoutcheck
grep -q 'echo "\[IMAGE\] build/toy_os.img published after disk assembly:' build.sh || fail early-marker
grep -q '^mv -f build/toy_os.img.final build/toy_os.img$' build.sh || fail final-atomic-publish
grep -q 'cmp -s build/disk.img build/toy_os.img' build.sh || fail final-identity
early=`grep -n '^mv -f build/toy_os.img.tmp build/toy_os.img$' build.sh | cut -d: -f1`
legacy=`grep -n '^\./check10.sh$' build.sh | head -1 | cut -d: -f1`
[ -n "$early" ] && [ -n "$legacy" ] && [ "$early" -lt "$legacy" ] || fail publication-order
! grep -q 'TESTEXE60\.TST' build.sh || fail stale-fat83-name
echo 'FIX60D IMAGE PUBLICATION CHECK: PASS'
