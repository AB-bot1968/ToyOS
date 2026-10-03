#!/bin/sh
# v64: copy must reject directories and the normalized source==destination case.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('static int fat_copy_file('); b=s.index('\n\n/*\n * Создаёт каталог',a); fn=s[a:b]
for needle in [
    'path_normalize(fat_cwd,source,src_abs,sizeof(src_abs))',
    'path_normalize(fat_cwd,dest,dst_abs,sizeof(dst_abs))',
    'if(sec[src_off+11]&0x10u)return 0;',
    'if(eq_path_for_copy(src_abs,target_abs))return 0;',
    'fat_resolve_parent(target_abs,&parent,leaf)',
    'fat_dir_find_free(parent,&dst_lba,&dst_off)',
    'fat_free_chain(new_first)',
]:
    if needle not in fn: raise SystemExit('missing: '+needle)
if 'fat_create_in_dir(parent,leaf' in fn: raise SystemExit('copy must not truncate destination through fat_create_in_dir')
PY
pass "v64 copy safety guards"
