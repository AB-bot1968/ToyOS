#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define SYS_FILE_SIZE   17u' src/kernel.c || fail 'kernel syscall number missing'
grep -q 'case SYS_FILE_SIZE' src/kernel.c || fail 'kernel dispatcher missing'
grep -q 'static int fat_file_size' src/kernel.c || fail 'FAT file-size helper missing'
grep -q 'SYS_FILE_SIZE' src/user_shell.c || fail 'shell syscall wrapper/test missing'
grep -q 'filesize FILE' src/user_shell.c || fail 'shell command help missing'
grep -q 'prefix(line,"filesize ")' src/user_shell.c || fail 'shell command dispatch missing'
test -f src/queue_tests/qsize.c || fail 'QSIZE source missing'
grep -q 'qsize' build.sh || fail 'QSIZE build missing'
grep -q 'QSIZE.EXE' build.sh || fail 'QSIZE install missing'
grep -q 'SYS_FILE_SIZE' SYSCALLS.md || fail 'syscall docs missing'
grep -q 'SYS_FILE_SIZE' README.md || fail 'README docs missing'
grep -q 'QSIZE.EXE' docs/EXE_QUEUE_TESTS_RU.md || fail 'EXE test docs missing'
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
assert 'size=handles[fd].size;' in s
assert 'fat_close(fd);' in s
assert 'if((f->cs&3u)&&!user_cstr(a))' in s
print('PASS: syscall 17 kernel path, Ring-3 shell, EXE1 test, error path, and docs')
PY
echo '=== CHECK32 PASS ==='
