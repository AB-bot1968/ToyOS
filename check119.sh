#!/bin/sh
set -eu
fail(){ echo "check119: FAIL: $1"; exit 1; }
grep -q 'telemetry_arp_reply' src/netdrv.c || fail 'ARP responder function missing'
grep -q 'be32(f+38)!=ip_u32(c->ip)' src/netdrv.c || fail 'ARP target-IP validation missing'
echo 'check119: PASS'
