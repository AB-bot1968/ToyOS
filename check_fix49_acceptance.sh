#!/bin/sh
set -eu
fail(){ echo "FIX49-ACC CHECK: FAIL: $*"; exit 1; }
grep -q 'RT_SLOT_PROGRESS' src/user_shell.c || fail 'RT progress ASSERT missing'
grep -q 'RT_DUMP_NONEMPTY' src/user_shell.c || fail 'RT dump ASSERT missing'
grep -q 'EVENT_SEQ_MONOTONIC' src/user_shell.c || fail 'event seq ASSERT missing'
grep -q 'EVENT_DISK_RECOVERY_CHAIN' src/user_shell.c || fail 'disk recovery ASSERT missing'
grep -q 'for t in TST/\*.TST TST/ACCEPT.TXT' build.sh || fail 'TST staging loop missing'
grep -q 'build/TST/TESTACC.TST=TST/TESTACC.TST' build.sh || fail 'TESTACC FAT entry missing'
grep -q 'ASSERT RT_SLOT_PROGRESS 1' TST/TESTACC.TST || fail 'RT progress test missing'
grep -q 'ASSERT EVENT_DISK_RECOVERY_CHAIN' TST/TESTACC.TST || fail 'persistent chain test missing'
grep -q 'ASSERT PROC_HANDLES 0' TST/TESTACC.TST || fail 'cleanup assertion missing'
echo 'FIX49-ACC CHECK: PASS'
