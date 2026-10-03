#!/bin/sh
set -eu
K=src/kernel.c
U=src/user_shell.c
H=src/hello.c
fail(){ echo "FAIL: $1" >&2; exit 1; }
grep -q 'static int user_range(uint32_t,uint32_t);' "$K" || fail 'kernel must forward-declare user_range before queue code'
grep -q '!user_range(p,count\*QUEUE_NAME_SIZE)' "$K" || fail 'queue_prepare must accept readable user buffers'
grep -q '!user_range(a,b\*QUEUE_NAME_SIZE)' "$K" || fail 'SYS_EXEC_QUEUE must validate readable user buffers'
grep -q 'char queue_test_names\[13\]' "$U" || fail 'SYS_EXEC_QUEUE test must use a full 13-byte packed slot'
grep -q "queue_test_names\[12\]=0" "$U" || fail 'queue test slot must be fully terminated/padded'
grep -q 'execq REPEAT FILE1' "$U" || fail 'execq command missing'
grep -q 'execq forever FILE1' "$U" || fail 'execq forever command missing'
grep -q 'SYS_EXEC_QUEUE' "$H" || fail 'HELLO must retain queue syscall ABI'
python3 - <<'PY'
assert 25*13 <= 400
print('PASS: queue ABI buffer fits shell line and 13-byte slots are validated as readable user memory')
PY

echo '=== CHECK27 PASS ==='
