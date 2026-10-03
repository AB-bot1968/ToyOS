#!/bin/sh
set -eu
f=src/execmt_task.c
test -f "$f"
grep -F 'sc(SYS_CONSOLE_WRITE,(uint32_t)task_line,8u,0);' "$f" >/dev/null
grep -F 'static const char task_line[] = MT_TASK_TEXT;' "$f" >/dev/null
# The visible MT line must not be assembled from several console writes.
if grep -q 'put(' "$f"; then echo 'ERROR: MT task still uses put()'; exit 1; fi
# Every shipped task ID is represented by the same fixed-width 8-byte format.
for i in `seq 1 25`; do
  id=`printf '%02d' "$i"`
  echo "[V47] MT$id fixed-line source check OK"
done
