#!/bin/sh
set -eu
f=src/netdrv.c
[ -f "$f" ]
grep -q 'UDP SLAVE running independently' "$f"
grep -q 'target_valid=0u' "$f"
grep -q 'SYS_CONSOLE_POLL' "$f"
printf '%s\n' 'check106: PASS'
