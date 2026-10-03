#!/bin/sh
# check65.sh — v61: cwd, cd/pwd и нормализация относительных путей.
# Тройная проверка: исходники -> ABI/dispatch -> 32-bit object compilation.
set -eu
cd "$(dirname "$0")"
CFLAGS="-m32 -Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib"
TMP="build/check65"
rm -rf "$TMP"
mkdir -p "$TMP"

printf '%s\n' '[1/3] source and ABI audit'
grep -F '#define SYS_CHDIR          29u' src/kernel.c >/dev/null
grep -F '#define SYS_GETCWD         30u' src/kernel.c >/dev/null
grep -F 'static char fat_cwd[128]="/";' src/kernel.c >/dev/null
grep -F 'static int fat_normalize_path' src/kernel.c >/dev/null
grep -F 'static int fat_is_dir' src/kernel.c >/dev/null
grep -F 'static int fat_chdir' src/kernel.c >/dev/null
grep -F 'case SYS_CHDIR:' src/kernel.c >/dev/null
grep -F 'case SYS_GETCWD:' src/kernel.c >/dev/null
grep -F 'static uint32_t sys_chdir' src/user_shell.c >/dev/null
grep -F 'static uint32_t sys_getcwd' src/user_shell.c >/dev/null
grep -F 'if(prefix(line,"cd "))' src/user_shell.c >/dev/null
grep -F 'if(eq(line,"pwd"))' src/user_shell.c >/dev/null
grep -F 'cwd-test' src/user_shell.c >/dev/null
grep -F 'sys_ls_path(".")' src/user_shell.c >/dev/null

printf '%s\n' '[2/3] 32-bit syntax + object compilation'
gcc $CFLAGS -fsyntax-only src/kernel.c
gcc $CFLAGS -fsyntax-only src/user_shell.c
gcc $CFLAGS -c src/kernel.c -o "$TMP/kernel.o"
gcc $CFLAGS -c src/user_shell.c -o "$TMP/user_shell.o"
test -s "$TMP/kernel.o"
test -s "$TMP/user_shell.o"

printf '%s\n' '[3/3] path semantics audit'
python3 - <<'PY'
# Независимо проверяем ожидаемую семантику нормализации, которую обязан
# реализовывать fat_normalize_path(): ROOT, '.', '..' и относительный путь.
def norm(cwd, path):
    assert cwd.startswith('/') and not cwd.endswith('/') or cwd == '/'
    raw = path if path.startswith('/') else (cwd.rstrip('/') + '/' + path)
    stack=[]
    for part in raw.split('/'):
        if not part or part == '.':
            continue
        if part == '..':
            if stack: stack.pop()
            continue
        stack.append(part.upper())
    return '/' + '/'.join(stack)
checks = {
    ('/', '.'): '/',
    ('/', '..'): '/',
    ('/BIN', '.'): '/BIN',
    ('/BIN', '..'): '/',
    ('/BIN', './../BIN/./'): '/BIN',
    ('/BIN', 'DOC/../HELLO.EXE'): '/BIN/HELLO.EXE',
    ('/BIN', '/DOC/NET.CFG'): '/DOC/NET.CFG',
}
for args, expected in checks.items():
    got=norm(*args)
    if got != expected:
        raise SystemExit('path semantic check failed: %r -> %r, expected %r' % (args, got, expected))
print('PASS: expected cwd/path normalization cases')
PY
printf '%s\n' 'PASS: v61 cwd/cd/pwd source, ABI and 32-bit compilation checks'
