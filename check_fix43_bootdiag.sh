#!/bin/sh
set -eu
grep -q 'BOOT_DIAG_NAME "BOOTSTAT.DAT"' src/user_shell.c
grep -q 'bootdiag_command' src/user_shell.c
grep -q 'BOOT_REASON ' src/user_shell.c
grep -q 'BOOT_MARKER_MISSING' src/user_shell.c
grep -q 'boot_diag_show();' src/user_shell.c
grep -q 'TST/TESTBOOT.TST=TST/TESTBOOT.TST' build.sh
grep -q 'RUN bootdiag mark 2' TST/TESTBOOT.TST
grep -q 'ASSERT BOOT_REASON 4' TST/TESTBOOT.TST
printf '%s\n' 'FIX43 boot diagnostics static check: OK'
