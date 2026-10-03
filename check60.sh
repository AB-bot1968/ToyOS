#!/bin/sh
# check60.sh — регрессионная проверка NETDRV v58 RX/TX.
set -eu
cd "$(dirname "$0")"

# RX ring должен начинаться с BNRY = PSTART - 1.
grep -F 'if(!ne_w(NE_BNRY,NE_RX_START-1u))return 0;' src/netdrv.c >/dev/null

# IPv4 Total Length должен быть ровно IP header + TCP header + data.
grep -F 'put_be16(ip+2,total);' src/netdrv.c >/dev/null
if grep -F 'put_be16(ip+2,(uint16_t)(20u+total));' src/netdrv.c >/dev/null; then
    echo 'FAIL: старая формула IPv4 Total Length осталась' >&2
    exit 1
fi

# QEMU/NE2000: broadcast + promiscuous receive. Это также устраняет
# зависимость от того, был ли MAC задан в командной строке QEMU.
grep -F 'if(!ne_w(NE_RCR,0x14u))return 0;' src/netdrv.c >/dev/null
if grep -F 'if(!ne_w(NE_RCR,0x01u))return 0;' src/netdrv.c >/dev/null; then
    echo 'FAIL: старое значение RCR=01 осталось' >&2
    exit 1
fi

# Remote DMA должен ожидать RDC.
grep -F 'static int ne_wait_rdc' src/netdrv.c >/dev/null
grep -F 'return ne_wait_rdc(timeout_ticks);' src/netdrv.c >/dev/null

# RX ring должен корректно читать через PSTOP -> PSTART.
grep -F 'static int ne_ring_read' src/netdrv.c >/dev/null

# Передача должна реально дополнять Ethernet frame до 60 байт.
grep -F 'while(old<60u)tx_frame[old++]=0;' src/netdrv.c >/dev/null

# TCP SYN+ACK: проверяем ACK нашего SYN.
grep -F 'if(be32(tcp+8)!=t->seq+1u)continue;' src/netdrv.c >/dev/null

# IPv4 входящих пакетов: проверяем Total Length и checksum.
grep -F 'if(csum(*ip,ihl_bytes)!=0u)return 0;' src/netdrv.c >/dev/null

echo 'PASS: NETDRV RX/TX regression checks'
