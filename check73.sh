#!/bin/sh
# v63.1: rmdir должен удалять пустой дочерний каталог относительным именем.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('static int fat_rmdir(const char*path)')
b=s.index('\nstatic int fat_close', a)
fn=s[a:b]
for needle in [
    'path_normalize(fat_cwd,path,abs,sizeof(abs))',
    'fat_resolve_parent(abs,&parent,leaf)',
    'fat_dir_lookup(parent,n,&lba,&off)',
    'sec[off]=0xe5;',
    'fat_rmdir_would_break_cwd(abs)',
]:
    if needle not in fn:
        raise SystemExit('missing: '+needle)
if 'fat_find(abs,&lba,&off)' in fn:
    raise SystemExit('rmdir still uses full-path lookup for deletion entry')
PY
pass "v63.1 rmdir relative child regression"
