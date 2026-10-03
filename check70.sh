#!/bin/sh
# v62.3: rmdir не должен удалять ROOT, текущий каталог или его родителя.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }

grep -q 'static int fat_rmdir_would_break_cwd' src/kernel.c || fail rmdir-cwd-guard-missing
grep -q 'path_normalize(fat_cwd,path,abs,sizeof(abs))' src/kernel.c || fail rmdir-does-not-normalize-path
grep -q 'fat_rmdir_would_break_cwd(abs)' src/kernel.c || fail rmdir-cwd-guard-not-called
grep -q "target\[0\]=='/'&&target\[1\]==0" src/kernel.c || fail rmdir-root-guard-missing
grep -q "target\[i\]==0&&fat_cwd\[i\]=='/'" src/kernel.c || fail rmdir-parent-guard-missing
pass "v62.3 rmdir cwd protection regression"
