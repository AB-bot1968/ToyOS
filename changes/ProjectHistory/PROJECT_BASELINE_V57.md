# Toy OS — Project Baseline v57

v57 продолжает проверенную v56.

## Новая функция

Добавлен пользовательский `LOADER.EXE`, который реализует автоматическую
цепочку:

```text
exec LOADER.EXE 1 RECV FF.EXE
        |
        +--> COMDRV.EXE 1 RECV FF.EXE
        |
        +--> FF.EXE
```

LOADER использует существующий `SYS_EXEC_QUEUE_ARGS`, а не новый формат EXE1.

## Минимальные изменения ядра

Изменены только два поведения:

1. `SYS_EXEC_QUEUE_ARGS` разрешён из активного Ring-3 EXE1.
2. Ненулевой `EBX` в `SYS_EXIT` завершает текущую очередь с ошибкой вместо
   запуска следующего элемента.

Это позволяет остановить автоматический запуск FF.EXE при ошибке COMDRV.

## Изменения COMDRV

`COMDRV.EXE` теперь завершает работу:

```text
SYS_EXIT(0) — успешная передача;
SYS_EXIT(1) — ошибка.
```

Сам UART, тайм-аут первого байта 10 секунд и межбайтовый idle timeout 1 секунда
не изменялись.

## Что не менялось

- формат EXE1;
- существующий SYS_EXEC;
- номера syscall 1..26;
- FAT16;
- scheduler;
- Ring-3 GDT/сегменты;
- Windows utility `win_sendfile.exe`;
- существующие команды shell.

## Результат

Новый путь:

```text
Windows -> TCP -> QEMU -> COM1 -> COMDRV.EXE -> FAT16 -> FF.EXE
```

запускается командой:

```text
exec LOADER.EXE 1 RECV FF.EXE
```
