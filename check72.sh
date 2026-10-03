#!/bin/sh
# v63: граничные случаи команд путей. Проверяем, что удаление файлов и каталогов
# используют единый канонический путь, а legacy raw lookup не остался именно в rm.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }

python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('static int fat_delete(const char*name)')
b=s.index('/*\n * Создаёт каталог', a)
fn=s[a:b]
if 'path_normalize(fat_cwd,name,abs,sizeof(abs))' not in fn:
    raise SystemExit('rm-delete-normalization-missing')
if 'fat_find(name,&lba,&off)' in fn:
    raise SystemExit('legacy-raw-delete-lookup-remains')
if 'if(sec[off+11]&0x10u)return 0;' not in fn:
    raise SystemExit('rm-must-reject-directories')
PY

grep -q 'if(prefix(line,"rm "))' src/user_shell.c || fail rm-command-missing
grep -q 'rm FILE' src/user_shell.c || fail rm-help-missing
grep -q 'path_normalize(fat_cwd,path,abs,sizeof(abs))' src/kernel.c || fail rmdir-normalization-missing
pass "v63 path edge-case regression"
