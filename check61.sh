#!/bin/sh
# check61.sh — v60: три независимых уровня проверки каталожного слоя.
# 1) статическая проверка исходника,
# 2) обычная компиляция объектного файла,
# 3) повторная компиляция с явным frame pointer.
set -eu
cd "$(dirname "$0")"

case "`gcc -dumpmachine`" in
    i[3-6]86-*|i686-*) ;;
    *) echo "ERROR: x86 W64DevKit required"; exit 1;;
esac

CFLAGS="-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib"
HOST="-O2 -std=c99 -Wall -Wextra -Werror"

echo '[CODE AUDIT 1/3] ABI and shell integration'
grep -F '#define SYS_MKDIR          27u' src/kernel.c >/dev/null
grep -F '#define SYS_RMDIR          28u' src/kernel.c >/dev/null
grep -F 'case SYS_MKDIR' src/kernel.c >/dev/null
grep -F 'case SYS_RMDIR' src/kernel.c >/dev/null
grep -F 'static uint32_t sys_ls(void)' src/user_shell.c >/dev/null
grep -F 'static uint32_t sys_ls_path' src/user_shell.c >/dev/null
grep -F '  ls\n  ls PATH\n  mkdir PATH\n  rmdir PATH' src/user_shell.c >/dev/null

echo '[CODE AUDIT 2/3] path and directory implementation'
grep -F 'static int fat_lookup_path' src/kernel.c >/dev/null
grep -F 'static int fat_resolve_parent' src/kernel.c >/dev/null
grep -F 'static int fat_dir_find_free' src/kernel.c >/dev/null
grep -F 'static int fat_mkdir' src/kernel.c >/dev/null
grep -F 'static int fat_rmdir' src/kernel.c >/dev/null
grep -F 'static int fat_free_chain' src/kernel.c >/dev/null
grep -F 'if(sec[off+11]&0x10u)return -1;' src/kernel.c >/dev/null

echo '[CODE AUDIT 3/3] image builder and verifier'
grep -F 'HOSTFILE[=DESTPATH]' tools/mkfat16.c >/dev/null
grep -F "d[0]='.'" tools/mkfat16.c >/dev/null
grep -F "d[32]='.'" tools/mkfat16.c >/dev/null
grep -F 'static int find_path' tools/fat16check.c >/dev/null
grep -F 'fat1' tools/fat16check.c >/dev/null
grep -F 'fat2' tools/fat16check.c >/dev/null
grep -F 'BIN/HELLO.EXE' build.sh >/dev/null
grep -F 'DOC/NET.CFG' build.sh >/dev/null

echo '[FS CHECK 1/3] syntax'
gcc -m32 $CFLAGS -fsyntax-only src/kernel.c
gcc -m32 $CFLAGS -fsyntax-only src/user_shell.c
gcc $HOST -fsyntax-only tools/mkfat16.c
gcc $HOST -fsyntax-only tools/fat16check.c

echo '[FS CHECK 2/3] object compilation'
gcc -m32 $CFLAGS -c src/kernel.c -o build/fs_kernel.check2.o
gcc -m32 $CFLAGS -c src/user_shell.c -o build/fs_shell.check2.o
gcc $HOST -c tools/mkfat16.c -o build/mkfat16.check2.o
gcc $HOST -c tools/fat16check.c -o build/fat16check.check2.o
test -s build/fs_kernel.check2.o
test -s build/fs_shell.check2.o

echo '[FS CHECK 3/3] object compilation with explicit frame pointer'
gcc -m32 $CFLAGS -fno-omit-frame-pointer -c src/kernel.c -o build/fs_kernel.check3.o
gcc -m32 $CFLAGS -fno-omit-frame-pointer -c src/user_shell.c -o build/fs_shell.check3.o
gcc $HOST -fno-omit-frame-pointer -c tools/mkfat16.c -o build/mkfat16.check3.o
gcc $HOST -fno-omit-frame-pointer -c tools/fat16check.c -o build/fat16check.check3.o
test -s build/fs_kernel.check3.o
test -s build/fs_shell.check3.o

echo 'PASS: v60 filesystem source and three compilation levels'
