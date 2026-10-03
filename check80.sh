#!/bin/sh
# v65.3: проверка комплекта сценариев сборки.
set -eu
for f in build.sh build.bat; do
    [ -f "$f" ] || { echo "FAIL: missing build entry: $f" >&2; exit 1; }
done
sh -n build.sh
grep -q 'sh -lc "./build.sh"' build.bat
echo "PASS: build.sh and build.bat are present and valid"
