#!/bin/sh
# v62.2: регрессия для ls через относительный путь ".".
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }

grep -q 'path_normalize(fat_cwd,path,abs,sizeof(abs))' src/kernel.c || fail ls-does-not-normalize-path
# ROOT после нормализации должен обрабатываться напрямую, без lookup_path(".").
grep -Fq "if(abs[0]!='/'||abs[1]!=0)" src/kernel.c || fail ls-root-normalized-check-missing
# Shell-команда ls обязана передавать ".", поэтому kernel должен принимать именно этот путь.
grep -q 'sys_ls_path(".")' src/user_shell.c || fail shell-ls-does-not-use-relative-dot
pass "v62.2 ls relative-dot regression"
