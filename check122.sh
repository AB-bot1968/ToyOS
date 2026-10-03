#!/bin/sh
set -eu
# UDP Ethernet frame length must include the 14-byte Ethernet header.
grep -q "return ne_send((uint16_t)(14u+total)" src/netdrv.c
echo "check122: PASS"
