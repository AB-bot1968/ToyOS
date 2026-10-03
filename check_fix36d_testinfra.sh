#!/bin/sh
set -eu
grep -q 'test_process_snapshot\[34u\*10u\]' src/user_shell.c
grep -q 'static char test_line\[400\]' src/user_shell.c
grep -q 'static char test_chunk\[256\]' src/user_shell.c
grep -q 'if(prefix(line,"test ")){test_command(line+5);continue;}' src/user_shell.c
grep -q 'nested test is not allowed' src/user_shell.c
! grep -A8 'static uint32_t test_assert' src/user_shell.c | grep -q 'v\[34u\*10u\]'
grep -q 'SYS_PROCESS_WAIT     56u' src/user_shell.c
grep -q 'TST/TESTCORE.TST' build.sh
printf '%s\n' 'FIX36D test stack safety static check: OK'
