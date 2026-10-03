#!/usr/bin/env bash
set -eu
f=src/user_shell.c
grep -q 'static void crlf_file' "$f"
grep -q 'FAT16_MODE_WRITE|FAT16_MODE_APPEND' "$f"
grep -q 'crlf: file not found/open failed' "$f"
grep -q 'crlf FILE' "$f"
grep -q '#define SYS_PROCESS_WAIT     56u' "$f"
echo 'FIX36E CRLF static check: OK'
