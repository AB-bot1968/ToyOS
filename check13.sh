#!/bin/sh
# check13.sh — EXE1-проверка без привязки к номеру записи ROOT.
# В v60 каталоги и README занимают переменное число root entries, поэтому
# старый тест с фиксированными LBA/cluster больше не является корректным.
set -eu
fail(){ echo "CHECK FAILED: $1" >&2; exit 1; }
B=build
[ -f "$B/HELLO.EXE" ] || fail HELLO.EXE
[ -f "$B/hello.pe" ] || fail hello.pe
[ -f "$B/hello.raw" ] || fail hello.raw
[ -f tools/mkexe.c ] || fail mkexe-source
[ "`wc -c < "$B/HELLO.EXE"`" -ge 17 ] || fail "EXE too small"
[ "`od -An -t x4 -N 4 "$B/HELLO.EXE" | tr -d ' \n'`" = 31455845 ] || fail "EXE1 magic"
[ "`od -An -t x4 -j 4 -N 4 "$B/HELLO.EXE" | tr -d ' \n'`" = 00100000 ] || fail "EXE entry address"
img_size=`od -An -t u4 -j 8 -N 4 "$B/HELLO.EXE" | tr -d ' '` 
bss_size=`od -An -t u4 -j 12 -N 4 "$B/HELLO.EXE" | tr -d ' '` 
raw_size=`wc -c < "$B/hello.raw"`
[ "$img_size" -eq "$raw_size" ] || fail "EXE image_size"
[ "$bss_size" -eq 0 ] || fail "HELLO.EXE BSS must be zero"
[ "`wc -c < "$B/HELLO.EXE"`" -le 2048 ] || fail "HELLO.EXE unexpectedly large"
# Не предполагаем фиксированную позицию записи: путь разрешается через FAT16.
[ -x "$B/fat16check.exe" ] || fail "fat16check.exe missing"
"$B/fat16check.exe" "$B/disk.img" HELLO.EXE >/dev/null || fail "HELLO.EXE root entry"
"$B/fat16check.exe" "$B/disk.img" BIN/HELLO.EXE >/dev/null || fail "BIN/HELLO.EXE entry"
grep -q 'SYS_EXEC' src/kernel.c || fail "kernel SYS_EXEC"
grep -q 'SYS_EXIT' src/kernel.c || fail "kernel SYS_EXIT"
grep -q 'exe_load' src/kernel.c || fail "EXE loader"
grep -q 'EXE_ERR_OPEN' src/kernel.c || fail "EXE diagnostic codes"
grep -q 'file_size-sizeof(h)' src/kernel.c || fail "EXE file-size validation"
grep -q 'exec: FAIL ' src/user_shell.c || fail "shell must show exec error code"
grep -q 'EXEC_STACK_PAGE>>12.*PAGE_P.*PAGE_RW.*PAGE_US' src/kernel.c || fail "EXE stack page mapping"
grep -q 'sys_exec' src/user_shell.c || fail "sys_exec wrapper"
grep -q 'exec ' src/user_shell.c || fail "exec command"
grep -q 'EXE1' docs/EXE_FORMAT_RU.md || fail "EXE documentation"
echo "13/13 checks passed"
