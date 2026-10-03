#!/bin/sh
set -eu
# ARP response fields: Ethernet type, ARP opcode=reply and local-IP target test.
grep -q "put_be16(tx_frame+20,2u)" src/netdrv.c
grep -q "if(be32(f+38)!=ip_u32(c->ip))return 0;" src/netdrv.c
echo "check121: PASS"
