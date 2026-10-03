# ToyOS v67.1 — Verification

Дата: 2026-09-19

## 1. Статическая проверка

Команда:

```text
./check127.sh
```

Результат:

```text
CHECK127 PASS: case-insensitive command parsing verified
```

Проверено наличие:

1. `ascii_fold()`;
2. case-insensitive `eq()`;
3. case-insensitive `prefix()`;
4. существующего `execute_line()`;
5. специальных `exec` handlers;
6. существующего path normalization;
7. совместимости с `SHELL_READ_UP` и историей команд.

## 2. Компиляция изменённых исходников

Полная `build.sh` была запущена, но остановлена штатной проверкой toolchain:

```text
ERROR: use the x86 W64DevKit (gcc target must be i686).
```

Это ожидаемое ограничение среды проверки: установленный GCC сообщает
`x86_64-linux-gnu`, тогда как проект требует x86 W64DevKit/i686.

Отдельно выполнена компиляция изменённых исходников 32-битным режимом
с теми же freestanding/no-libc/no-SSE требованиями:

```text
gcc -m32 -Os -ffreestanding -fno-pie -fno-stack-protector \
  -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin \
  -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 \
  -mno-mmx -mno-80387 -nostdinc -nostdlib -c src/user_shell.c
```

Результат: `PASS`.

Аналогично скомпилирован `src/path.c`: `PASS`.

## 3. Runtime-план проверки в W64DevKit/QEMU

После штатной сборки следует проверить:

```text
help
HELP
HeLp
ls
LS
pwd
PWD
exec RDT.EXE ...
EXEC rdt.exe ...
execq ...
EXECQ ...
```

Для каждой команды ожидается одинаковое распознавание имени команды.

Отдельно проверить аргументы:

```text
write Test.TXT AbCdEf
```

Ожидается, что регистр аргументов не будет shell-ом изменён.

Для истории:

```text
rDt.ExE
↑
```

Ожидается восстановление именно `rDt.ExE`.
