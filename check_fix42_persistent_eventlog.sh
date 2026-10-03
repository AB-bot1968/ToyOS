#!/bin/sh
set -eu
grep -q 'EVENT_DISK_NAME "EVENT.LOG"' src/user_shell.c
grep -q 'event_disk_checksum' src/user_shell.c
grep -q 'eq(p,"save")' src/user_shell.c
grep -q 'eq(p,"disk")' src/user_shell.c
grep -q 'eq(p,"clear disk")' src/user_shell.c
grep -q 'EVENT_DISK_REASON' src/user_shell.c
grep -q 'EVENT_DISK_COUNT' src/user_shell.c
grep -q 'EVENT_DISK_MISSING' src/user_shell.c
grep -q 'TST/TESTLOG.TST=TST/TESTLOG.TST' build.sh
grep -q 'ASSERT EVENT_DISK_REASON 2' TST/TESTLOG.TST
grep -q 'ASSERT EVENT_DISK_MISSING' TST/TESTLOG.TST
# Persistence must remain outside kernel fault/IRQ paths.
! grep -q 'EVENT.LOG' src/kernel.c
echo 'FIX42 persistent eventlog static check: OK'
