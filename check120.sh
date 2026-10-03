#!/bin/sh
set -eu
grep -q "10.66.1.2" NET_MAST.CFG
grep -q "10.66.2.2" NET_SLV1.CFG
grep -q "UDP MASTER listening" src/netdrv.c
grep -q "UDP SLAVE running independently" src/netdrv.c
echo "check120: PASS"
