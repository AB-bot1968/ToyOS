#!/bin/sh
set -eu
fail(){ echo "FIX60B BUILD FINALIZATION CHECK: FAIL: $1" >&2; exit 1; }
grep -q 'FINAL ARTIFACT GATE' build.sh || fail final-gate
grep -q 'test -s build/toy_os.img' build.sh || fail image-existence-gate
grep -q 'cmp -s build/disk.img build/toy_os.img' build.sh || fail image-identity-gate
grep -q 'layoutcheck.exe build/toy_os.img' build.sh || fail image-layout-gate
grep -q 'fat16check.exe build/toy_os.img SONARDRV.EXE' build.sh || fail sonar-in-final-image
grep -q 'BUILD AND VERIFICATION OK' build.sh || fail success-marker
final_line=`grep -n '^mv -f build/toy_os.img.final build/toy_os.img$' build.sh | tail -1 | cut -d: -f1`
modern_line=`grep -n '^./check_fix60a_executable_loader.sh$' build.sh | tail -1 | cut -d: -f1`
[ -n "$final_line" ] && [ -n "$modern_line" ] && [ "$final_line" -gt "$modern_line" ] || fail final-publish-order
echo 'FIX60B BUILD FINALIZATION CHECK: PASS'
