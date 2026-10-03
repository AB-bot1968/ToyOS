#!/bin/sh
set -eu
S=src/user_shell.c
K=src/kernel.c
grep -q 'put("  mtlist                   list active background MT tasks' "$S"
grep -q 'if(eq(line,"mtlist")){mtlist_command();return 0;}' "$S"
! grep -q 'if(eq(line,"mtstat"))' "$S"
grep -q 'q\[1\]!=1u&&q\[1\]!=2u&&q\[1\]!=3u' "$S"
grep -q '#define SYS_MT_STATUS       51u' "$K"
grep -q '#define SYS_MT_STOP_ONE    52u' "$K"
echo 'PASS: FIX14 MTLIST shell view; MT ABI 51/52 preserved'
