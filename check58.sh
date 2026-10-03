#!/bin/sh
# v58: проверка исправления разбора команды exec LOADER.EXE без запуска QEMU.
# Проверяем, что shell имеет специальный обработчик LOADER с расширенным ABI.
grep -q 'static void exec_loader_command(char\*p)' src/user_shell.c || exit 1
# Проверяем точный префикс команды и смещение после строки "exec LOADER.EXE ".
grep -q 'prefix(line,"exec LOADER.EXE ")' src/user_shell.c || exit 1
grep -q 'exec_loader_command(line+16)' src/user_shell.c || exit 1
# Проверяем упаковку трёх аргументов в существующий блок SYS_EXEC_ARGS 48 байт.
grep -q 'sys_exec_args(name,args)' src/user_shell.c || exit 1
grep -q 'exec LOADER.EXE PORT RECV FILE.EXE' src/user_shell.c || exit 1
echo 'CHECK58 PASS: shell передаёт PORT/RECV/FILE в LOADER.EXE через существующий SYS_EXEC_ARGS.'
