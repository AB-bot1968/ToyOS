#!/bin/sh
# v64: paginated help must expose cp and all page navigation without hiding old commands.
set -eu
python3 - <<'PY'
from pathlib import Path
s=Path('src/user_shell.c').read_text()
required=[
'help [PAGE]', 'cp SOURCE DEST', 'ls [PATH]', 'mkdir PATH', 'rmdir PATH',
'exec FILE.EXE', 'execmt FILE1.EXE', 'execq-stop', 'syscall N [argument]', 'exit'
]
for x in required:
    if x not in s: raise SystemExit('help missing: '+x)
for x in ['page==1u','page==2u','page==3u','page<4u','page>4u']:
    if x not in s: raise SystemExit('help pagination guard missing: '+x)
PY
echo "PASS: v64 paginated help boundary regression"
