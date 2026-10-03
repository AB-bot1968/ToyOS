#!/bin/sh
set -eu
f=src/netdrv.c
grep -q 'telemetry_rx_recover' "$f"
grep -q '10u&&ip\[1\]==66u&&ip\[2\]==2u&&ip\[3\]==2u' "$f"
grep -q '10u&&ip\[1\]==66u&&ip\[2\]==2u&&ip\[3\]==3u' "$f"
grep -q 'if(r<0)' "$f"
printf 'check113: PASS\n'
