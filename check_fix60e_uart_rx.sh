#!/usr/bin/env bash
set -euo pipefail
K=src/kernel.c
S=src/sonardrv.c
grep -q 'static uint32_t uart_irq_time_us(void){return rt_time_ticks\*10000u;}' "$K"
! grep -q 'uart_last_irq_us=serial_time_us()' "$K"
! grep -q 'uart_last_rx_us=serial_time_us()' "$K"
grep -q 'if(op==8u)' "$K"
grep -q 'uart_rx_tail=uart_rx_head' "$K"
grep -q 'drained<UART_RX_CAP' "$K"
grep -q 'SYS_UART_TRANSPORT,0u,8u,0u' "$S"
grep -q 'FIX60E KEY ISOLATION PRE-CLEAN' TST/TESTKEY.TST
grep -q 'TST/TESTSEQ.TST=TST/TESTSEQ.TST' build.sh
grep -q 'TST/TESTSEQ.TST' build.sh
grep -q '#define SYS_PORT_OUT8[[:space:]]*13u' "$K"
grep -q '#define SYS_PORT_IN8[[:space:]]*14u' "$K"
echo 'FIX60E UART RX HARDENING CHECK: PASS'
