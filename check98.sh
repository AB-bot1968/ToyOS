#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
python3 - "$ROOT/src/user_shell.c" <<'PY'
import sys
p=sys.argv[1]
s=open(p,encoding='utf-8').read()
start=s.index('static uint32_t execute_line(char*line)')
end=s.index('\n}\n\n/*\n * v65.11: /AUTOSTART.SH', start)
body=s[start:end]
assert 'readline(' not in body, 'execute_line must not read keyboard input'
assert 'put("toy0> ")' not in body, 'execute_line must not print interactive prompt'
assert 'trim(line)' in body, 'execute_line must trim supplied command'
loop=s[s.index('static void shell_loop'):]
assert 'put("toy0> ")' in loop, 'interactive prompt must remain in shell_loop'
print('PASS: shared execute_line accepts supplied lines without consuming keyboard input')
PY
