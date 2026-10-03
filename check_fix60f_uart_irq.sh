#!/bin/sh
set -eu
K=src/kernel.c
fail(){ echo "FIX60F UART IRQ CHECK: FAIL: $1" >&2; exit 1; }
grep -q '#define UART_IRQ_BURST 8u' "$K" || fail burst
grep -q 'service exactly one 16550 interrupt reason per IRQ entry' "$K" || fail one-reason
! grep -q 'static void uart1_irq(void){uint32_t guard=32u' "$K" || fail old-loop
grep -q 'while(k<UART_IRQ_BURST)' "$K" || fail rx-bound
grep -q 'while(k<UART_IRQ_BURST&&uart_tx_tail' "$K" || fail tx-bound
grep -q 'RUN mtstop ALL' TST/TESTLOAD.TST || fail load-cleanup
grep -q 'RUN safemode normal 0' TST/TESTLOAD.TST || fail load-normal
grep -q 'TST/TESTIRQ.TST=TST/TESTIRQ.TST' build.sh || fail test-pack
grep -q 'TST/TESTIRQ.TST' build.sh || fail test-verify
echo 'FIX60F UART IRQ CHECK: PASS'
