# Проверка Toy OS v65.6

## Проверено

1. `src/kernel.c` — i386 freestanding compilation с проектными флагами.
2. `src/vgadrv.c` — i386 freestanding compilation с проектными флагами.
3. `src/user_shell.c` — i386 freestanding compilation.
4. `src/path.c` — i386 freestanding compilation.
5. `tools/mkfat16.c`, `tools/mkexe.c`, `tools/fat16check.c` — hosted `-Wall -Wextra -Werror`.
6. `resources/SPLASH.RAW` — ровно 64000 байт.
7. `build.sh` содержит проверки `check84.sh` и `check85.sh`.
8. Старой функции Ring-3 `set_text_mode()` в VGADRV больше нет.
9. `SYS_VIDEO_TEXT = 35` присутствует и в kernel syscall dispatcher, и в VGADRV.

## Ограничение среды

Полная сборка и запуск QEMU в текущей среде невозможны: здесь отсутствуют Windows 7/W64DevKit x86 и QEMU i386. Поэтому boot/runtime-тест возврата VGA не выдаётся за фактически выполненный здесь.
