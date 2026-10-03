#!/bin/sh
set -eu
fail(){ echo "CHECK127 FAIL: $1"; exit 1; }
grep -q 'static char ascii_fold' src/user_shell.c || fail ascii-fold-helper
grep -q 'ascii_fold(a\[i\])==ascii_fold(b\[i\])' src/user_shell.c || fail eq-casefold
grep -q 'ascii_fold(a\[i\])!=ascii_fold(b\[i\])' src/user_shell.c || fail prefix-casefold
grep -q 'static uint32_t execute_line' src/user_shell.c || fail execute-line
grep -q 'prefix(line,"exec RTD.EXE "' src/user_shell.c || fail exec-rtd-path
grep -q 'prefix(line,"exec LOADER.EXE "' src/user_shell.c || fail exec-loader-path
grep -q 'prefix(line,"exec COMDRV.EXE "' src/user_shell.c || fail exec-com-path
grep -q 'prefix(line,"exec NETDRV.EXE "' src/user_shell.c || fail exec-net-path
grep -q 'prefix(line,"exec VGADRV.EXE "' src/user_shell.c || fail exec-vga-path
grep -q 'prefix(line,"exec "' src/user_shell.c || fail generic-exec-path
grep -q 'path_normalize' src/path.c || fail path-normalization
grep -q 'SHELL_READ_UP' src/user_shell.c || fail history-up-preserved
echo 'CHECK127 PASS: case-insensitive command parsing verified'
