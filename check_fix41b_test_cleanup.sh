#!/usr/bin/env bash
set -euo pipefail
fail(){ echo "FIX41B cleanup check: FAIL: $*"; exit 1; }
for f in TST/TESTHB.TST TST/TESTWD.TST TST/TESTSUP.TST TST/TESTRES.TST TST/TESTKEY.TST TST/TESTLOAD.TST TST/TESTEVT.TST; do
  grep -qi '^RUN mtstop ALL$' "$f" || fail "$f does not close MT session"
done
grep -q 'ToyOS v67.11-B FIX41B TEST SESSION CLEANUP' VERSION.txt || fail VERSION
[ ! -d build ] || fail 'build/ must not be in source archive'
echo 'FIX41B test cleanup static check: OK'
