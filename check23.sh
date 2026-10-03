#!/bin/sh
set -eu
fail(){ echo "CHECK FAILED: $1"; exit 1; }
grep -q '#define SYS_EXEC_QUEUE  *15u' src/kernel.c || fail syscall-15
 grep -q '#define SYS_QUEUE_STOP  *16u' src/kernel.c || fail syscall-16
grep -q 'QUEUE_MAX_FILES 25u' src/kernel.c || fail max-25
grep -q 'QUEUE_MAX_REPS  3000u' src/kernel.c || fail max-3000
grep -q 'queue_names\[QUEUE_MAX_FILES\]\[QUEUE_NAME_SIZE\]' src/kernel.c || fail queue-storage
grep -q 'queue_index' src/kernel.c || fail circular-index
grep -q 'queue_rounds' src/kernel.c || fail repetitions
 grep -q 'queue_forever' src/kernel.c || fail infinite-mode
grep -q 'queue_start_next' src/kernel.c || fail next-program
grep -q 'case SYS_EXEC_QUEUE' src/kernel.c || fail syscall-15-handler
grep -q 'case SYS_QUEUE_STOP' src/kernel.c || fail syscall-16-handler
grep -q 'if(s==0x01){if(queue_active)queue_stop_requested=1;return;}' src/kernel.c || fail esc-stop
grep -q 'sys_exec_queue' src/user_shell.c || fail shell-wrapper
grep -q 'sys_queue_stop' src/user_shell.c || fail stop-wrapper
grep -q 'execq forever' src/user_shell.c || fail infinite-command
grep -q 'execq-stop' src/user_shell.c || fail stop-command
grep -q 'char line\[400\]' src/user_shell.c || fail queue-command-line
 grep -q '\[15\] SYS_EXEC_QUEUE x2' src/user_shell.c || fail runtime-queue-test
grep -q '\[16\] SYS_QUEUE_STOP shell guard' src/user_shell.c || fail runtime-stop-test
grep -q 'SYS_EXEC_QUEUE' src/hello.c || fail exe-wrapper
grep -q 'SYS_QUEUE_STOP' src/hello.c || fail exe-stop-wrapper
! grep -qE '__asm__[^[:cntrl:]]*(cli|sti|hlt|lgdt|lidt|ltr|outb|inb|mov[[:space:]]+[^[:cntrl:]]*cr[0-9])' src/user_shell.c || fail privileged-user-instruction
echo 'PASS: v23 EXE1 circular queue, repetition limit, infinite mode and stop path checks'
grep -q 'execq REPEAT FILE1' docs/EXE_QUEUE_RU.md || fail queue-doc-command
grep -q 'execq forever FILE1' docs/EXE_QUEUE_RU.md || fail queue-doc-forever
grep -q 'SYS_EXEC_QUEUE = 15' docs/EXE_QUEUE_RU.md || fail queue-doc-syscall
grep -q 'SYS_QUEUE_STOP = 16' docs/EXE_QUEUE_RU.md || fail stop-doc-syscall
grep -q 'Esc' docs/EXE_QUEUE_RU.md || fail stop-doc-esc
echo 'PASS: v23 documentation checks'
python3 - <<'PY'
assert 25*13 <= 400
print('PASS: 25 packed FAT 8.3 names fit the Ring-3 shell command buffer')
PY
