#!/bin/sh
set -eu
fail(){ echo "check112: FAIL: $1"; exit 1; }
test "$(cat VERSION.txt)" = "Toy OS v66.11" || fail 'VERSION.txt'
grep -q '10.66.1.2' NET_MAST.CFG || fail 'master subnet'
grep -q '10.66.2.2' NET_SLV1.CFG || fail 'slave1 subnet'
grep -q '10.66.2.3' NET_SLV2.CFG || fail 'slave2 subnet'
grep -q 'Wi-Fi' README_V66_3_WIFI_RU.md || fail 'README'
test -s SET_WIFI_PC1_V66_3.BAT || fail 'PC1 setup script'
test -s SET_WIFI_PC2_V66_3.BAT || fail 'PC2 setup script'
echo 'check112: PASS'
