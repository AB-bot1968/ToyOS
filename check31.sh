#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define SYS_FILE_SIZE   17u' src/kernel.c || fail 'kernel syscall number missing'
grep -q 'case SYS_FILE_SIZE' src/kernel.c || fail 'kernel dispatcher missing'
grep -q 'sys_file_size' src/user_shell.c || fail 'Ring-3 wrapper missing'
grep -q 'filesize FILE' src/user_shell.c || fail 'shell help missing'
test -f src/queue_tests/qsize.c || fail 'QSIZE.EXE source missing'
grep -q 'qsize' build.sh || fail 'QSIZE not built/installed'
grep -q 'SYS_FILE_SIZE' SYSCALLS.md || fail 'syscall documentation missing'
grep -q 'SYS_FILE_SIZE' README.md || fail 'README documentation missing'
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
assert 'static int fat_file_size(const char*name)' in s
assert 'size=handles[fd].size;' in s
assert 'fat_close(fd);' in s
print('PASS: SYS_FILE_SIZE implementation, shell path, EXE test, and docs are present')
PY
echo '=== CHECK31 PASS ==='
