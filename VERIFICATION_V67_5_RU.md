# ToyOS v67.5 — Verification

## Статические проверки

PASS:
- `VGADRV.EXE` после возврата в text mode переводит cursor на строку 24;
- сообщение `VGADRV: OK` выводится без LF, чтобы shell prompt продолжился в той же нижней строке;
- старый вывод `VGADRV: OK\n` удалён;
- `SYS_CONSOLE_AT` не изменён;
- `SYS_VIDEO_TEXT` не изменён;
- syscall ABI не изменён.

## Компиляция

PASS:
- `src/vgadrv.c` — freestanding 32-bit compile;
- `src/kernel.c` — freestanding 32-bit compile.

## Ограничение среды

Полная `build.sh` не запускалась в данной среде: проектный build требует i686/W64DevKit toolchain. Это не выдаётся за результат полной сборки.
