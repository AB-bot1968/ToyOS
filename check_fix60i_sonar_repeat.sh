#!/bin/sh
set -eu
K=src/kernel.c
S=src/sonardrv.c
fail(){ echo "FIX60I SONAR REPEAT CHECK: FAIL: $1"; exit 1; }
grep -q 'FIX60I: EXECMT is detached/background' "$K" || fail quiet-comment
# Background scheduler announcement may update metadata but must not print.
python3 - <<'PY'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('static void sched_announce')
b=s.index('static int sched_pick_next', a)
body=s[a:b]
assert 'console_write' not in body
assert 'announced=1' in body
PY
grep -q 'transaction RX flush must clear both the software ring' "$K" || fail hw-flush-comment
line=$(grep 'if(op==8u)' "$K")
printf '%s\n' "$line" | grep -q 'uart_rx_tail=uart_rx_head' || fail sw-flush
printf '%s\n' "$line" | grep -q 'inb(UART1_BASE+5u)' || fail lsr-poll
printf '%s\n' "$line" | grep -q 'inb(UART1_BASE)' || fail hw-drain
printf '%s\n' "$line" | grep -q 'drained<UART_RX_CAP' || fail bounded-drain
python3 - <<'PY2'
from pathlib import Path
s=Path('src/kernel.c').read_text()
a=s.index('if(op==8u)')
b=s.index('/* op5/op6', a)
assert 'uart1_set_tx_irq' not in s[a:b]
assert 'outb(UART1_BASE+1u' not in s[a:b]
PY2
grep -q 'send_request:.*SYS_UART_TRANSPORT,0u,8u,0u' "$S" || fail per-attempt-flush
grep -q 'for(;;).*send_request:' "$S" || fail cyclic-loop
echo 'FIX60I SONAR REPEAT CHECK: PASS'
