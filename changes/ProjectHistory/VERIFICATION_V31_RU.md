# v31 — проверка исправления EXC 14 при `exec QNESTED.EXE`

## Найденная причина

В v30 при запуске `exec QNESTED.EXE` EXE1 уже имел сохранённый `saved_user_frame`, указывающий обратно в Ring-3 shell.
Однако `queue_prepare()` ошибочно перезаписывал этот frame текущим frame самого `QNESTED.EXE`.

Затем `SYS_EXEC_QUEUE` отключал user-доступ к текущему image и запускал первый элемент дочерней очереди. После завершения очереди `queue_finish()` восстанавливал перезаписанный frame `QNESTED.EXE`.
EIP этого frame находился около `0x00100036`, но страницы QNESTED уже имели `U/S=0`, поэтому процессор получил:

`#PF (EXC 14), CR2 == EIP, CS=0x1B`.

Это точно соответствует наблюдавшемуся виду ошибки `EXC 14 EIP=1048630 CS=27 CR2=1048630` (`1048630 == 0x00100036`, `27 == 0x1B`).

## Исправление

`queue_prepare()` теперь сохраняет shell frame только при одновременных условиях:

- нет активной очереди;
- нет активного EXE.

То есть:

- shell → `execq`: shell frame сохраняется;
- standalone EXE → `SYS_EXEC_QUEUE`: уже сохранённый shell frame не трогается;
- EXE внутри `execq` → `SYS_EXEC_QUEUE`: shell frame не трогается, а frame состояния родительской очереди помещается в `queue_stack`.

После завершения standalone очереди восстановление возвращает shell, а не отключённый image `QNESTED.EXE`.

## Статическая проверка

Проверено:

1. новый регрессионный `check30.sh`;
2. `check26.sh` — layout kernel/FAT16;
3. `check28.sh` — nested queue, EXE1, FAT16;
4. `check29.sh` — stack состояния nested queue и EXE-origin detection;
5. строгая локальная 32-битная компиляция `kernel.c`, `user_shell.c`, `hello.c` и всех `queue_tests/*.c` с `-Wall -Wextra -Werror`;
6. строгая компиляция host tools `mkfat16`, `fat16check`, `mkexe`.

Полная PE/COFF сборка именно W64DevKit и запуск QEMU в текущем Linux-аудите недоступны, поэтому результат runtime на Windows 7 не объявляется как выполненный.
