#!/bin/sh
set -eu
F=src/netdrv.c
if command -v gcc >/dev/null 2>&1; then
  gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -Wall -Wextra -Werror -fsyntax-only "$F"
else
  echo 'check124: SKIP (gcc unavailable)'
  exit 0
fi
echo 'check124: PASS'
