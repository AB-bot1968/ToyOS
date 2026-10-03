# VERIFICATION v49

## Исправление EXECMT Task NN

В v49 исправлена проблема, при которой `execmt mt01.exe ... mt04.exe`
не показывал строку `Task NN`, хотя диагностическое сообщение планировщика
могло присутствовать.

### Изменение

Строка `Task NN\n` каждой EXECMT-программы теперь принудительно размещается
в отдельной входной секции `.usercode_data`, которая linker `program.ld`
включает непосредственно в выходную секцию `.text` raw EXE.

Это исключает зависимость видимого сообщения от отдельного output-section
`.rodata` и гарантирует, что байты строки находятся внутри того же raw image,
который `mkexe` помещает после EXE1 header и который `execmt_load_one()`
загружает в private image slot задачи.

Исполняемый файл по-прежнему сам вызывает:

    SYS_CONSOLE_WRITE(task_line, 8)

То есть сообщение `Task NN` не генерируется ядром вместо программы.

### Сохранено

- `exec`, `execq`, `execmt`;
- `ls` и меню shell;
- существующие syscall ABI;
- отдельный CR3 для EXECMT-задач;
- отдельные физические image slots и stacks;
- диагностические строки планировщика;
- Ring-3 shell;
- SYS_SCHED_BLOCK / SYS_SCHED_WAKE / SYS_SCHED_EXIT;
- формат EXE1.

## Статическая проверка

`check49.sh` проверяет наличие `.usercode_data`, его включение в linker,
единый `SYS_CONSOLE_WRITE`, корректные Ring-3 DS/ES/FS/GS и наличие команд
`exec`, `execq`, `execmt`, `ls`.

Дополнительно исходник `execmt_task.c` проверен в i386 freestanding режиме
для всех EXECMT_ID=1..25.

Полная штатная сборка W64DevKit/QEMU в Linux-окружении не выполняется,
поскольку проект требует x86 W64DevKit/i686 toolchain.
