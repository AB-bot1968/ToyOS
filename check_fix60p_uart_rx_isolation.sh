#!/bin/sh
set -eu
fail(){ echo "FIX60P CHECK FAIL: $*" >&2; exit 1; }
K=src/kernel.c; S=src/user_shell.c; D=src/sonardrv.c; R=src/uartrx.c; B=build.sh
[ -f "$R" ] || fail "missing UARTRX source"
grep -q 'if(op==11u)' "$K" || fail "UART op11 diagnostics missing"
grep -q 'uart_hw_rx_bytes' "$K" || fail "physical RX counter missing"
grep -q 'uart_lsr_oe' "$K" || fail "LSR error counters missing"
grep -q 'uart_rx_max_queued' "$K" || fail "RX high-water missing"
grep -q 'uartstat_command' "$S" || fail "uartstat shell command missing"
grep -q 'uartreset' "$S" || fail "uartreset cleanup command missing"
grep -q 'RUN uartreset' TST/TESTRXP.TST || fail "TESTRXP does not clean UART state"
grep -q 'SYS_UART_TRANSPORT,0u,7u,0u).*UART_TEST_FAIL("DISABLE")' "$S" || fail "TESTUART does not force offline mode"
grep -q 'DATA_PUBLISHED_WAIT' "$S" || fail "bounded endurance wait missing"
grep -q 'ASSERT DATA_PUBLISHED_WAIT 0 1000 1000' TST/TESTSONP.TST || fail "1000-cycle endurance criterion missing"
grep -q 'ASSERT MT_HEARTBEAT 1 1000' TST/TESTSONP.TST || fail "1000 heartbeat criterion missing"
grep -q 'Keep transport disabled' "$D" || fail "SONARTST offline isolation missing"
grep -q 'SYS_DATA_CHANNEL' "$R" && fail "UARTRX must not use Data Channel"
grep -q 'mb_parse' "$R" && fail "UARTRX must not parse Modbus response"
grep -q 'SYS_PROCESS_HEARTBEAT' "$R" || fail "UARTRX heartbeat missing"
grep -q 'build/UARTRX.EXE=UARTRX.EXE' "$B" || fail "UARTRX not packaged"
grep -q 'build/TST/TESTSONP.TST=TST/TESTSONP.TST' "$B" || fail "TESTSONP not packaged"
grep -q 'build/TST/TESTRXP.TST=TST/TESTRXP.TST' "$B" || fail "TESTRXP not packaged"
grep -q './check_fix60p_uart_rx_isolation.sh' "$B" || fail "checker not in build gate"
grep -q 'FIX60P — UART RX isolation' TOYOS_ARCHITECTURE_VISION.md || fail "Architecture Vision not updated"
# Physical polling architecture remains mandatory.
grep -q 'outb(UART1_BASE+1u,0u)' "$K" || fail "IER=0 policy missing"
! grep -q 'irq_enable(4)' "$K" || fail "PIC IRQ4 must remain masked"
# User-approved unrestricted port syscall ABI must remain present.
grep -q 'case SYS_PORT_OUT8' "$K" || fail "syscall13 removed"
grep -q 'case SYS_PORT_IN8' "$K" || fail "syscall14 removed"
echo 'FIX60P UART RX ISOLATION CHECK: PASS'
