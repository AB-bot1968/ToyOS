#!/bin/sh
set -eu
K=src/kernel.c; U=src/user_shell.c; B=build.sh; T=TST/TESTUART.TST
fail(){ echo "FIX55 UART static checker: FAIL: $1" >&2; exit 1; }
grep -q '#define SYS_UART_TRANSPORT      70u' "$K" || fail syscall
grep -q 'else if(n==36)uart1_irq' "$K" || fail handler
grep -q 'UART_RX_CAP 256u' "$K" || fail rxcap
grep -q 'uart_rx_overrun' "$K" || fail overrun
grep -q 'serial_time_us' "$K" || fail legacy_time
grep -q 'uart_transport_enabled=0u' "$K" || fail default_off
grep -q 'case SYS_PORT_OUT8' "$K" || fail portout
grep -q 'case SYS_PORT_IN8' "$K" || fail portin
grep -q 'EXPECT UART_TRANSPORT' "$T" || fail test
grep -q 'for t in TST/\*.TST TST/ACCEPT.TXT' "$B" || fail pack
grep -q 'test_expect_uart_transport' "$U" || fail expect
# FIX60H/N supersedes the original THRE-IRQ activation policy: physical syscall-70 is polled.
grep -q 'static uint32_t uart1_poll_tx_hw' "$K" || fail polltx
grep -q 'if(a){uart1_reset();outb(UART1_BASE+1u,0u);}' "$K" || fail ierzero
if grep -q 'if(k&&uart_transport_enabled)uart1_set_tx_irq(1u)' "$K"; then fail stale_thre_enable; fi
echo 'FIX55 UART static checker: PASS (FIX60N polling evolution accepted)'
