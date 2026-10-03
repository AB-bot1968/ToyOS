#!/bin/sh
set -eu
fail(){ echo "check111: FAIL: $1"; exit 1; }
grep -q '0x1bu' src/netdrv.c || fail 'ESC handling missing'
grep -q 'UDP MASTER listening' src/netdrv.c || fail 'master mode missing'
grep -q 'UDP SLAVE running independently' src/netdrv.c || fail 'slave independence missing'
test -s WIFI_ROUTING_V66_3_RU.md || fail 'routing documentation missing'
echo 'check111: PASS'
