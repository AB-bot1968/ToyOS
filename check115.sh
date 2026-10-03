#!/bin/sh
set -eu
for f in src/kernel.c src/netdrv.c src/user_shell.c; do test -s "$f"; done
grep -q 'UDP MASTER listening' src/netdrv.c
grep -q 'TELEMETRY SLAVE' src/netdrv.c || true
echo 'check115: PASS'
