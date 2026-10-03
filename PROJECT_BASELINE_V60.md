# Toy OS v60 — baseline

v60 сохраняет архитектуру v59: 32-bit BIOS/protected mode, Ring-3 shell,
INT 80h, paging, FAT16, EXE1, scheduler и NETDRV.

Главное изменение v60 — иерархическая FAT16 filesystem с каталогами.

## ABI

- `SYS_LS=24`: `EBX=PATH`, `0` означает root;
- `SYS_MKDIR=27`: `EBX=PATH`;
- `SYS_RMDIR=28`: `EBX=PATH`.

`SYS_FILE_OPEN=6`, `SYS_FILE_DELETE=10`, `SYS_FILE_SIZE=17` и `SYS_EXEC=11`
принимают 8.3-пути, включая вложенные каталоги.

## Build

Целевая среда остаётся прежней: Windows 7 + x86 W64DevKit + 32-bit target.
Основной `build.sh` намеренно требует `gcc -dumpmachine` семейства `i686`.
`build.bat` запускается из W64DevKit shell.

## FAT16 layout

- boot/FAT16 volume: LBA 66;
- root directory: фиксированные 512 entries;
- subdirectories: обычные FAT16 cluster chains;
- directory entry attribute `0x10`;
- `.` указывает на собственный первый кластер;
- `..` указывает на кластер родителя, для root — `0`.

## Следующий этап

v61: shell `cd`/`pwd`, cwd и нормализация `.`/`..` без изменения дискового
формата.
