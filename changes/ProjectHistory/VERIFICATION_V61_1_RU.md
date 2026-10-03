# Проверка Toy OS v61.2

Проверены исходные изменения для увеличения kernel slot до 128 секторов.

## Статические проверки

- `sh -n build.sh`;
- синтаксис всех изменённых check-скриптов;
- 32-битный `-fsyntax-only` для `src/kernel.c` и `src/user_shell.c`;
- host-сборка `mkfat16`, `fat16check`, `mkexe` с `-Wall -Wextra -Werror`.

## Boot

- raw boot sector — 512 байт;
- сигнатура 55 AA;
- два DAP-запроса по 64 сектора;
- первый запрос LBA 1, второй LBA 65;
- второй буфер 1000:7E00.

## FAT16

- FAT16 boot sector — LBA 130;
- kernel slot — LBA 1..128;
- scratch — LBA 129;
- тестовые файлы успешно обнаруживаются в ROOT и вложенных каталогах.

Полную целевую сборку Windows 7 + W64DevKit следует выполнить в вашей целевой среде.
