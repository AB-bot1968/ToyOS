# PROJECT BASELINE V58

v58 основан непосредственно на v57.

## Единственное функциональное изменение

Исправлен shell-разбор:

    exec LOADER.EXE PORT RECV FILE.EXE

Теперь shell передаёт три аргумента LOADER.EXE через уже существующий
`SYS_EXEC_ARGS`, аналогично существующему `exec COMDRV.EXE`.

## Ядро

`src/kernel.c` не изменён относительно v57.

## COMDRV

Не изменён относительно v57/v56:

- COM1..COM4;
- SEND/RECV;
- ожидание первого байта 10 секунд;
- межбайтовый idle timeout 1 секунда.

## LOADER

`LOADER.EXE` из v57 не изменён.

## Проверяемая команда

    exec LOADER.EXE 1 RECV FF.EXE

Цепочка:

    LOADER.EXE -> COMDRV.EXE -> FF.EXE

## Цель v58

Исправить только потерю аргументов на уровне shell, не добавляя новый
syscall и не меняя ABI ядра.
