#!/bin/sh
set -eu
K=src/kernel.c
S=src/user_shell.c
grep -q 'static volatile uint32_t rt_launch_guard;' "$K"
grep -q 'if(rt_launch_guard)return;' "$K"
grep -q 'if(op==7u){rt_launch_guard=a?1u:0u' "$K"
grep -q 'sys_rt_stats((void\*)1,7u);' "$S"
grep -q 'sys_rt_stats((void\*)0,7u);' "$S"
echo 'PASS: FIX24 F10 RT/MT launch guard static checks'
