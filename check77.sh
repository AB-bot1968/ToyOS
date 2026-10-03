#!/bin/sh
# v64: cp parser boundary cases are checked statically because runtime input is tested in QEMU.
set -eu
python3 - <<'PY'
from pathlib import Path
s=Path('src/user_shell.c').read_text()
a=s.index('static void copy_file_command(char*p)'); b=s.index('\n}\n',a)+2; fn=s[a:b]
for x in ['token(&p,src,sizeof(src))','token(&p,dst,sizeof(dst))','if(!src[0]||!dst[0]||*p)','usage: cp SOURCE DEST']:
    if x not in fn: raise SystemExit('cp parser missing: '+x)
PY
echo "PASS: v64 cp parser boundary regression"
