#!/bin/sh
set -eu
printf '[check104] UDP telemetry configuration examples\n'
for f in NET_MAST.CFG NET_SLV1.CFG NET_SLV2.CFG SET_NET_M.BAT SET_NET_1.BAT SET_NET_2.BAT; do test -s "$f" || { echo "FAIL: missing $f"; exit 1; }; done
grep -q 'IP=192.168.100.10' NET_MAST.CFG
grep -q 'IP=192.168.100.11' NET_SLV1.CFG
grep -q 'IP=192.168.100.12' NET_SLV2.CFG
grep -q 'TELEMETRY MASTER 5000' VERIFICATION_V66_RU.md
grep -q 'OFFLINE' VERIFICATION_V66_RU.md
printf '[check104] PASS\n'
