#!/bin/sh
# v24: strict source compilation gate. This catches warnings that would
# otherwise be easy to miss before the W64DevKit PE/COFF link stage.
set -eu
case "`gcc -dumpmachine`" in
    i[3-6]86-*|i686-*) ;;
    *) echo "FAIL: use the x86 W64DevKit (gcc target must be i686)."; exit 1;;
esac
mkdir -p build/check24
CFLAGS="-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib -Wall -Wextra -Werror"
gcc $CFLAGS -c src/kernel.c -o build/check24/kernel.o
gcc $CFLAGS -c src/user_shell.c -o build/check24/user_shell.o
gcc $CFLAGS -c src/hello.c -o build/check24/hello.o
for src in src/queue_tests/*.c; do gcc $CFLAGS -c "$src" -o "build/check24/$(basename "$src" .c).o"; done
as --32 src/boot.S -o build/check24/boot.o
as --32 src/isr.S -o build/check24/isr.o
rm -rf build/check24
echo "PASS: v24 strict W64DevKit source compilation (C + GNU as)"
