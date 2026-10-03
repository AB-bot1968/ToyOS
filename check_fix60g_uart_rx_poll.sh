#!/bin/sh
set -eu
K=src/kernel.c
fail(){ echo "FIX60G UART RX POLL CHECK: FAIL: $*" >&2; exit 1; }
grep -q '#define UART_POLL_BURST 32u' "$K" || fail burst
grep -q 'static uint32_t uart1_poll_rx_hw' "$K" || fail helper
grep -q 'uart1_poll_rx_hw();' "$K" || fail read_path
grep -q 'f->eax=uart1_rx_pop' "$K" || fail read_pop
grep -q 'uart_transport_enabled=a;if(a){uart1_reset();outb(UART1_BASE+1u,0u);}' "$K" || fail rx_irq_disabled
# No enable path may explicitly write IER=1 (RX available interrupt).
if grep -q 'outb(UART1_BASE+1u,1u)' "$K"; then fail rx_irq_enable_still_present; fi
grep -q 'if(op==9u){f->eax=(uint32_t)inb(UART1_BASE+1u);break;}' "$K" || fail ier_probe
# Intentional port syscalls remain untouched.
grep -q 'case SYS_PORT_OUT8:' "$K" || fail syscall13
grep -q 'case SYS_PORT_IN8:' "$K" || fail syscall14
echo 'FIX60G UART RX POLL CHECK: PASS'
