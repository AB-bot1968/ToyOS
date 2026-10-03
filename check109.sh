#!/bin/sh
set -eu
fail(){ echo "check109: FAIL: $1"; exit 1; }
grep -q 'IP=10.66.1.2' NET_MAST.CFG || fail 'master IP'
grep -q 'GATEWAY=10.66.1.1' NET_MAST.CFG || fail 'master gateway'
grep -q 'IP=10.66.2.2' NET_SLV1.CFG || fail 'slave1 IP'
grep -q 'GATEWAY=10.66.2.1' NET_SLV1.CFG || fail 'slave1 gateway'
grep -q 'IP=10.66.2.3' NET_SLV2.CFG || fail 'slave2 IP'
grep -q 'GATEWAY=10.66.2.1' NET_SLV2.CFG || fail 'slave2 gateway'
grep -q 'route_ip' src/netdrv.c || fail 'routed Wi-Fi path missing'
echo 'check109: PASS'
