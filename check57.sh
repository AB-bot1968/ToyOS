#!/bin/sh
# v57 source-level regression check for LOADER.EXE and the minimal kernel changes.
set -eu
fail(){ echo "CHECK57 FAIL: $*" >&2; exit 1; }
# LOADER must use the existing extended queue ABI.
grep -q 'SYS_EXEC_QUEUE_ARGS 26u' src/loader.c || fail 'LOADER syscall 26 missing'
grep -q 'COMDRV.EXE' src/loader.c || fail 'LOADER COMDRV entry missing'
grep -Fq 'copy_field(job0+QUEUE_NAME_SIZE,port' src/loader.c || fail 'LOADER port argument packing missing'
grep -Fq 'copy_field(job0+QUEUE_NAME_SIZE+EXEC_ARG_SIZE,dir' src/loader.c || fail 'LOADER direction argument packing missing'
grep -Fq 'copy_field(job0+QUEUE_NAME_SIZE+(2u*EXEC_ARG_SIZE),file' src/loader.c || fail 'LOADER filename argument packing missing'
grep -q 'copy_field(job1,file' src/loader.c || fail 'LOADER target EXE entry missing'
grep -Fq '"=b"(*port)' src/loader.c || fail 'LOADER EBX ABI read missing'
grep -Fq '"=c"(*dir)' src/loader.c || fail 'LOADER ECX ABI read missing'
grep -Fq '"=d"(*file)' src/loader.c || fail 'LOADER EDX ABI read missing'
# Kernel must permit argument queues from an active EXE1.
grep -q 'queue_prepare_args(uint32_t p,uint32_t count,uint32_t repetitions,struct frame\*f,uint32_t from_exe)' src/kernel.c || fail 'nested argument queue signature missing'
grep -q 'uint32_t from_exe=(f->cs&3u)&&exe_active' src/kernel.c || fail 'nested argument queue dispatch missing'
grep -q 'if(from_exe&&queue_active)queue_save_parent' src/kernel.c || fail 'nested parent queue save missing'
# Queue failure must prevent the next EXE1 from starting.
grep -q 'uint32_t status=f->ebx' src/kernel.c || fail 'SYS_EXIT status source missing'
grep -q 'if(status!=0u){queue_finish(f,status);return;}' src/kernel.c || fail 'queue failure stop missing'
# COMDRV must report transfer errors to the queue.
grep -q 'sc(SYS_EXIT,ok?0u:1u,0,0)' src/comdrv.c || fail 'COMDRV status propagation missing'
# The new source files must remain freestanding.
if gcc -m32 -ffreestanding -fsyntax-only -fno-pie -fno-stack-protector -nostdinc src/loader.c; then :; else fail 'LOADER syntax'; fi
if gcc -m32 -ffreestanding -fsyntax-only -fno-pie -fno-stack-protector -nostdinc src/comdrv.c; then :; else fail 'COMDRV syntax'; fi
echo 'CHECK57 PASS'
