# Проверка Toy OS v65.7

- `SPLASH.RAW` = 16000 байт (160x100x256 RAW).
- `VGADRV.EXE` компилируется в i386 freestanding режиме без предупреждений при проектных флагах.
- `kernel.c` компилируется после добавления VGA-state save/restore.
- `mkfat16.c`, `mkexe.c`, `fat16check.c`, `pathcheck` проходят строгие hosted-проверки.
- `check68.sh` ... `check88.sh` проходят статическую регрессию.
- В `SYS_VIDEO_MAP` перед предоставлением VRAM вызывается `vga_save_state()`.
- В `SYS_VIDEO_TEXT` вызывается `vga_restore_text_mode_hw()` с проверкой результата.
- Восстановление включает 21 Attribute Controller регистр и всю DAC-палитру.

Полный runtime-тест QEMU/Windows 7 в этой среде не выполняется.
