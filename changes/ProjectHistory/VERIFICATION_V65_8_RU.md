# Проверка Toy OS v65.8

Проверено локально:

- `resources/SPLASH.RAW` — ровно 64000 байт;
- `resources/SPLASH_PREVIEW.png` — 320x200;
- `src/kernel.c` — i386 freestanding compilation;
- `src/vgadrv.c` — i386 freestanding compilation;
- `src/user_shell.c` — i386 freestanding compilation;
- `src/path.c` — i386 freestanding compilation;
- `tools/mkfat16.c`, `tools/mkexe.c` — строгая hosted-компиляция;
- `build.sh`, `check78.sh`..`check89.sh` — shell syntax;
- статические `check88.sh` и `check89.sh` — PASS.

Аппаратный запуск Windows 7 + W64DevKit x86 + QEMU в текущей среде не выполнялся.
