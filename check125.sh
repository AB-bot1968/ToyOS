#!/bin/sh
set -eu
F=src/netdrv.c
grep -q 'TOY OS | UDP TELEMETRY MASTER' "$F"
grep -q 'TOY OS | UDP TELEMETRY SLAVE' "$F"
grep -q 'SLAVE1 ]  IP=10.66.2.2' "$F"
grep -q 'SLAVE2 ]  IP=10.66.2.3' "$F"
echo 'check125: PASS'
