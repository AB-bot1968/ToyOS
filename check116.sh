#!/bin/sh
set -eu
fail(){ echo "check116: FAIL: $1"; exit 1; }
grep -q '4294967293' VERIFICATION_V66_5_RU.md || fail 'unsigned -3 documentation missing'
grep -q 'EXE_ERR_OPEN' VERIFICATION_V66_5_RU.md || fail 'EXE_ERR_OPEN documentation missing'
grep -q 'const char bin_name\[\]="/BIN/NETDRV.EXE"' src/user_shell.c || fail 'BIN path missing'
grep -q 'r=sys_exec_args(bin_name,args)' src/user_shell.c || fail 'BIN execution missing'
grep -q 'r=sys_exec_args(name,args)' src/user_shell.c || fail 'root fallback missing'
grep -q 'exec NETDRV.EXE IP SEND|RECV FILE.TXT' src/user_shell.c || fail 'legacy command syntax changed'
printf 'check116: PASS\n'
