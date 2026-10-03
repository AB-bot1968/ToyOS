#!/bin/sh
# v63: регрессия команды rm и общего разрешения путей для удаления файлов.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }

grep -q 'static int fat_delete' src/kernel.c || fail fat-delete-missing
grep -q 'path_normalize(fat_cwd,name,abs,sizeof(abs))' src/kernel.c || fail rm-does-not-use-canonical-path
grep -q 'if(prefix(line,"rm "))' src/user_shell.c || fail rm-command-missing
grep -q 'rm FILE' src/user_shell.c || fail rm-help-missing
grep -q 'if(sec\[off+11\]&0x10u)return 0;' src/kernel.c || fail rm-must-reject-directories
pass "v63 rm path/delete regression"
