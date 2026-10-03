#!/bin/sh
set -eu
fail(){ echo "check110: FAIL: $1"; exit 1; }
grep -q 'arp_resolve(&probe,route_ip)' src/netdrv.c || fail 'slave must ARP selected route'
grep -q 'ip_send_udp(c,dst_ip' src/netdrv.c || fail 'UDP destination must remain Master IP'
grep -q 'IPEnableRouter' SET_WIFI_PC1_V66_3.BAT || fail 'PC1 routing setup missing'
grep -q 'IPEnableRouter' SET_WIFI_PC2_V66_3.BAT || fail 'PC2 routing setup missing'
echo 'check110: PASS'
