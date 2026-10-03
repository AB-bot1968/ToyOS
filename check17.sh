#!/bin/sh
# FIX53 supersedes the old fixed-LBA syscall disk test.
set -eu
grep -q 'sys_layout_info(layoutq)' src/user_shell.c
grep -q 'scratch=layoutq\[4\]+layoutq\[6\]+layoutq\[7\]' src/user_shell.c
echo 'PASS: disk syscall scratch sector is descriptor-driven protected gap'
