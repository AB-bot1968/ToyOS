#!/bin/sh
set -eu
fail(){ echo "FIX60Q CHECK FAIL: $*" >&2; exit 1; }
R=src/uartrx.c; B=build.sh; S=tools/sonarsim/sonarsim.c; T=TST/TESTRXP.TST
[ -f "$R" ] && [ -f "$S" ] || fail "sources missing"
grep -q 'UART_RX_TEST_INJECT' "$R" || fail "offline UARTRX test build missing"
grep -q 'gcc .*UART_RX_TEST_INJECT=1.*uartrx.c' "$B" || fail "UARTRXT compile missing"
grep -q 'build/UARTRXT.EXE=UARTRXT.EXE' "$B" || fail "UARTRXT not packaged"
grep -q 'RUN execmt UARTRXT.EXE' "$T" || fail "TESTRXP still launches physical host backend"
! grep -q 'RUN execmt UARTRX.EXE' "$T" || fail "automatic TST must not launch physical UARTRX"
grep -q 'ASSERT MT_PROGRESS 1 3' "$T" || fail "scheduler progress criterion missing"
grep -q 'ASSERT MT_HEARTBEAT 1 10' "$T" || fail "RX lifecycle heartbeat criterion missing"
grep -q 'ASSERT UART_ENABLED 0' "$T" || fail "offline transport criterion missing"
grep -q -- '--tcp' "$S" || fail "SONARSIM TCP mode missing"
grep -q 'run_tcp' "$S" || fail "SONARSIM TCP runtime missing"
grep -q 'bounded-socket-io' "$S" || fail "TCP READY diagnostic missing"
grep -q -- '-lws2_32' tools/sonarsim/build_w64devkit.bat || fail "Winsock link missing"
grep -q 'FIX60Q — host serial boundary' TOYOS_ARCHITECTURE_VISION.md || fail "Architecture Vision not updated"
# Kernel UART ABI and user-approved raw port syscalls are deliberately unchanged.
grep -q 'case SYS_UART_TRANSPORT' src/kernel.c || fail "UART syscall removed"
grep -q 'case SYS_PORT_OUT8' src/kernel.c || fail "syscall13 removed"
grep -q 'case SYS_PORT_IN8' src/kernel.c || fail "syscall14 removed"
echo 'FIX60Q QEMU SOCKET BOUNDARY CHECK: PASS'
