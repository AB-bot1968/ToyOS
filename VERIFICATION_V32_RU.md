# Verification v32 — SYS_FILE_SIZE

## Новая функциональность

Добавлен системный вызов `SYS_FILE_SIZE = 17`.

ABI:

- `EAX = 17`;
- `EBX = указатель на NUL-terminated имя FAT16 8.3`;
- `ECX/EDX` не используются;
- `EAX` после `INT 80h` содержит точный размер файла в байтах;
- `EAX = 0xffffffff` означает отсутствие файла или ошибку.

Системный вызов работает как из Ring-3 shell, так и непосредственно из EXE1.
Он не требует пользовательского file descriptor и не оставляет открытый handle.

## Shell

Добавлены:

```text
filesize README.TXT
syscall 17 README.TXT
```

Для несуществующего файла ожидается `filesize: FAIL`.

## EXE1

Добавлен `QSIZE.EXE`. Он вызывает `SYS_FILE_SIZE` для `README.TXT`, выводит
полученное количество байт, затем проверяет отказ для `NOFILE.TXT` и завершает
EXE через `SYS_EXIT`.

## Статические проверки

1. `check30.sh` — регрессия исправления standalone `QNESTED.EXE`.
2. `check31.sh` — наличие syscall 17, shell wrapper, QSIZE и документации.
3. `check32.sh` — kernel dispatcher, FAT helper, shell command, EXE1 test,
   обработка отсутствующего файла и документация.
4. Строгая 32-битная компиляция kernel/user/EXE1 sources с `-Wall -Wextra -Werror`.
5. Строгая компиляция host tools `mkfat16`, `fat16check`, `mkexe`.
6. Тестовая FAT16 image с `QSIZE.EXE`; `fat16check` успешно проверил все
   установленные EXE1 и размеры directory entries.

Полная сборка PE/COFF именно W64DevKit и запуск QEMU в текущей Linux-среде
не выполнялись, поэтому Windows 7/QEMU runtime PASS не заявляется.
