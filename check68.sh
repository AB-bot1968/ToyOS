#!/bin/sh
# v62: проверка, что обработка путей вынесена в единый модуль и подключена к kernel.
set -eu
fail(){ echo "FAIL: $1" >&2; exit 1; }
pass(){ echo "PASS: $1"; }

test -f src/path.c || fail path-c-source-missing
test -f src/path.h || fail path-h-header-missing
grep -q '"path.h"' src/kernel.c || fail kernel-does-not-include-path-h
grep -q 'src/path.c -o build/path.o' build.sh || fail path-object-not-built
grep -q 'build/path.o build/isr.o' build.sh || fail path-object-not-linked
grep -q 'path_normalize(fat_cwd' src/kernel.c || fail kernel-does-not-use-path-normalizer
if grep -q 'static int eq_str' src/kernel.c; then fail duplicate-path-string-helper-remains; fi
if grep -q 'fat_normalize_path' src/kernel.c; then fail legacy-path-normalizer-remains; fi
for fn in path_is_absolute path_normalize path_join path_parent path_basename; do
    grep -q "int $fn" src/path.c || fail missing-$fn
done
pass "v62 standalone path module is present and linked"
