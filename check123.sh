#!/bin/sh
set -eu
F=src/netdrv.c
grep -q 'static int telemetry_source_role(const uint8_t ip\[4\],const uint8_t\*data,uint16_t n)' "$F"
grep -q 'role=telemetry_source_role(ip,data,ul)' "$F"
grep -q 'NETDRV: UDP RX SLAVE1 count=' "$F"
grep -q 'NETDRV: UDP RX SLAVE2 count=' "$F"
grep -q 'telemetry_render_line(7,"SLAVE1",payload1,len1)' "$F"
grep -q 'telemetry_render_line(9,"SLAVE2",payload2,len2)' "$F"
! grep -q 'telemetry_source_role(ip)' "$F"
echo 'check123: PASS'
