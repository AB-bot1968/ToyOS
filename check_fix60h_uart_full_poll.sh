#!/bin/sh
set -eu
K=src/kernel.c
S=src/sonardrv.c
grep -q 'static uint32_t uart1_poll_tx_hw' "$K"
grep -q 'neither RX nor TX can generate IRQ4' "$K"
grep -q 'if(op==10u){f->eax=rt_time_ticks\*10000u;break;}' "$K"
grep -q 'SYS_UART_TRANSPORT,0u,10u,0u' "$S"
# Physical op2 must not arm THRE IRQ.
line=`grep 'if(op==2u)' "$K"`
printf '%s\n' "$line" | grep -q 'uart1_poll_tx_hw'
printf '%s\n' "$line" | grep -qv 'uart1_set_tx_irq'
# Enable path keeps IER at zero.
grep -q 'if(a){uart1_reset();outb(UART1_BASE+1u,0u);}' "$K"
echo 'FIX60H UART FULL POLL CHECK: PASS'
grep -q '^EXPECT UART_FULL_POLL$' TST/TESTPHY.TST
grep -q 'build/TST/TESTPHY.TST=TST/TESTPHY.TST' build.sh
