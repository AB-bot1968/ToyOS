#!/bin/sh
# v64: syscall cp, shell command and paginated help must be wired together.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }
python3 - <<'PY'
from pathlib import Path
k=Path('src/kernel.c').read_text(); sh=Path('src/user_shell.c').read_text()
for text,name in [(k,'kernel'),(sh,'shell')]:
    if '#define SYS_FILE_COPY      31u' not in text:
        raise SystemExit(f'{name}: missing SYS_FILE_COPY')
if 'case SYS_FILE_COPY:' not in k: raise SystemExit('kernel: missing copy dispatcher')
if 'sys_file_copy(const char*src,const char*dst)' not in sh: raise SystemExit('shell: missing copy wrapper')
if 'cp SOURCE DEST' not in sh: raise SystemExit('shell: missing cp command')
if 'help_page(uint32_t page)' not in sh or 'help_command(char*p)' not in sh: raise SystemExit('shell: missing paginated help')
if 'page+1u' not in sh or 'page>4u' not in sh: raise SystemExit('shell: help pages are not navigable')
PY
pass "v64 cp syscall and paginated help wiring"
