#!/bin/sh
set -eu
f=src/kernel.c
[ -f "$f" ]
grep -q '#define SYS_CONSOLE_POLL   37u' "$f"
grep -q 'key_push(0x1bu)' "$f"
grep -q 'case SYS_CONSOLE_POLL' "$f"
printf '%s\n' 'check107: PASS'
