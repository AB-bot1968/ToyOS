#!/bin/sh
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q '#define SYS_EXECMT      23u' src/kernel.c || fail 'SYS_EXECMT 23 missing'
grep -q '#define SYS_LS          24u' src/kernel.c || fail 'SYS_LS 24 missing'
grep -q 'static int fat_ls' src/kernel.c || fail 'FAT root listing missing'
grep -q 'console_write_color(name,0x01u)' src/kernel.c || fail 'blue ls output missing'
grep -q 'case SYS_LS' src/kernel.c || fail 'SYS_LS dispatcher missing'
grep -q 'static uint32_t sys_ls(void)' src/user_shell.c || fail 'shell SYS_LS wrapper missing'
grep -q 'if(eq(line,"ls"))' src/user_shell.c || fail 'ls command missing'
grep -q '  ls\\n' src/user_shell.c || fail 'ls missing from help'
grep -q 'execmt FILE1.EXE \[FILE2.EXE ... FILE25.EXE\]' src/user_shell.c || fail 'execmt missing from help'
grep -q '#define SYS_LS 24u' src/queue_tests/qpass.c || fail 'QPASS SYS_LS missing'
grep -q 'calling SYS_LS' src/queue_tests/qpass.c || fail 'QPASS does not exercise SYS_LS'
grep -q 'v43' README.md || fail 'README v43 missing'
python3 - <<'PY'
from pathlib import Path
s=Path('src/user_shell.c').read_text()
assert '  ls\\n' in s
assert '  execmt FILE1.EXE [FILE2.EXE ... FILE25.EXE]\\n' in s
k=Path('src/kernel.c').read_text()
assert '#define SYS_LS          24u' in k
assert 'if(!first)console_put(\' \');' in k
assert 'console_write_color(name,0x01u)' in k
print('PASS: v43 help/ls/SYS_LS source checks')
PY
echo 'PASS: check43.sh'
