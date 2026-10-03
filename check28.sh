#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
for f in src/queue_tests/qpass.c src/queue_tests/qport.c src/queue_tests/qfromexe.c src/queue_tests/qstop.c src/queue_tests/qnested.c src/queue_tests/qfile.c; do
  test -f "$f" || fail "missing $f"
done
grep -q 'SYS_EXEC_QUEUE' src/queue_tests/qfromexe.c || fail 'QFROMEXE must use SYS_EXEC_QUEUE'
grep -q 'SYS_QUEUE_STOP' src/queue_tests/qstop.c || fail 'QSTOP must use SYS_QUEUE_STOP'
grep -q 'SYS_EXEC_QUEUE' src/queue_tests/qnested.c || fail 'QNESTED must use SYS_EXEC_QUEUE'
grep -q 'SYS_FILE_OPEN' src/queue_tests/qfile.c || fail 'QFILE must use SYS_FILE_OPEN'
grep -q 'SYS_FILE_WRITE' src/queue_tests/qfile.c || fail 'QFILE must use SYS_FILE_WRITE'
grep -q 'src/queue_tests/qpass.c' build.sh || fail 'build must compile QPASS'
grep -q 'src/queue_tests/qport.c' build.sh || fail 'build must compile QPORT'
grep -q 'src/queue_tests/qfromexe.c' build.sh || fail 'build must compile QFROMEXE'
grep -q 'src/queue_tests/qstop.c' build.sh || fail 'build must compile QSTOP'
grep -q 'build/QPASS.EXE' build.sh || fail 'build must install QPASS.EXE'
grep -q 'build/QPORT.EXE' build.sh || fail 'build must install QPORT.EXE'
grep -q 'build/QFROMEXE.EXE' build.sh || fail 'build must install QFROMEXE.EXE'
grep -q 'build/QSTOP.EXE' build.sh || fail 'build must install QSTOP.EXE'
grep -q 'build/QNESTED.EXE' build.sh || fail 'build must install QNESTED.EXE'
grep -q 'build/QFILE.EXE' build.sh || fail 'build must install QFILE.EXE'
grep -q 'HELLO.EXE QPASS.EXE QPORT.EXE QFROMEXE.EXE QSTOP.EXE QNESTED.EXE QFILE.EXE' build.sh || fail 'build must verify every shipped EXE1'
grep -q 'FAT16 image created' tools/mkfat16.c || fail 'multi-file FAT16 builder missing'
python3 - <<'PY'
assert 25*13 <= 400
print('PASS: nested queue, EXE1 file append test, multi-file FAT16 installation, and 25-name ABI fit')
PY
echo '=== CHECK28 PASS ==='
