#!/bin/sh
set -eu
F=src/netdrv.c
grep -q 'ARP: WAITING' "$F"
grep -q 'TRANSMIT: 0 packets' "$F"
grep -q 'LAST TELEMETRY: waiting for first transmission' "$F"
grep -q 'ESC = stop telemetry master' "$F"
echo 'check126: PASS'
