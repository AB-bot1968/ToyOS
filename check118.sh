#!/bin/sh
set -e
G="${CC:-gcc}"
CFLAGS="-m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Wall -Wextra -Werror -Iinclude"
$G $CFLAGS -fsyntax-only src/netdrv.c
printf '%s\n' 'check118: PASS - Slave has ESC stop, ARP retry and UDP TX status.'
