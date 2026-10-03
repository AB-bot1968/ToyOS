# Toy OS v65.8

## Исправление VGA splash и возврата в текстовый режим

В v65.8 графический ресурс возвращён к нативному VGA-размеру 320x200x256 (64000 байт). Больше нет искусственного увеличения 160x100 до 320x200 блоками 2x2.

Для возврата в консоль `SYS_VIDEO_TEXT` теперь выполняет полный стандартный VGA mode-set 03h: Screen Off, Sequencer reset, Misc Output, Sequencer, разблокировка/CRTC, Graphics Controller, Attribute Controller, PEL mask, текстовая палитра и только затем снятие reset/включение экрана. Сохранение произвольного VGA-state удалено как ненадёжное.

Команда остаётся неизменной:

```text
exec VGADRV.EXE SPLASH.RAW
```

После заставки `VGADRV` ждёт один символ через `SYS_CONSOLE_READ`; любой символ вызывает `SYS_VIDEO_TEXT`, после чего VRAM mapping снимается и драйвер завершается.
