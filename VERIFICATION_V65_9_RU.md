# Проверка Toy OS v65.9

## Исправление VGA text mode

Проверено, что kernel сохраняет VGA 8x16 font из plane 2 до запуска graphics mode:

- `vga_saved_font[256*16]` — 4096 байт;
- `vga_save_font()` вызывается сразу после `zero_bss()`;
- используется специальная VGA plane-2 адресация (`SEQ 02=04`, `SEQ 04=06`, `GC 05=00`, `GC 06=04`).

После `SYS_VIDEO_TEXT` проверено:

1. стандартная конфигурация VGA mode 03h 80x25;
2. `SEQ register 04 = 07`;
3. восстановление `font plane 2`;
4. только затем `console_clear()` и обновление курсора.

Это устраняет артефакты символов после возврата из Mode 13h.

## Ресурс

`resources/SPLASH.RAW`:

- 320x200;
- 256 индексов цвета;
- 64000 байт;
- палитра RAW соответствует палитре, которую программирует `VGADRV.EXE`.

## Статические проверки

`check78.sh` ... `check92.sh` — PASS.

## Компиляция

`kernel.c` и `vgadrv.c` проверены как i386 freestanding; `vgadrv.c`, `path.c` и host tools дополнительно проверены с `-Wall -Wextra -Werror`.

В текущей Linux-среде полная сборка Windows 7/W64DevKit не выполняется, потому что локальный GCC имеет target `x86_64`; штатный `build.sh` это корректно диагностирует.
