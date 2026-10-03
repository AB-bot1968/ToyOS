# Toy OS v65.1 — проверка исправления сборки

## Исправленная ошибка

В v65 `build.sh` передавал `build/SPLASH.RAW=SPLASH.RAW` в `mkfat16`,
но не создавал `build/SPLASH.RAW`. При строгом `set -eu` сборка поэтому
останавливалась на этапе добавления графического файла.

## Проверено

- `sh -n build.sh` — PASS.
- `sh -n check78.sh` — PASS.
- `resources/SPLASH.RAW` — 64000 байт.
- staging `resources/SPLASH.RAW -> build/SPLASH.RAW` — PASS.
- строгая проверка размера `build/SPLASH.RAW == 64000` — PASS.
- `tools/mkfat16.c` с `-O2 -std=c99 -Wall -Wextra -Werror` — PASS.
- `mkfat16` создал FAT16 с `SPLASH.RAW` — PASS.
- `fat16check SPLASH.RAW` подтвердил 64000 байт и 125 кластеров — PASS.
- `check78.sh` — PASS.
- i386 freestanding compilation `kernel.c`, `user_shell.c`, `path.c`, `vgadrv.c`
  и остальных пользовательских исходников — PASS.

Полная сборка W64DevKit x86/Windows 7 и запуск QEMU в текущей среде не выполнялись:
локальная среда Linux x86_64, а проект намеренно требует i686/i386 W64DevKit.

## Примечание для v65.2

Запуск VGADRV выявил отдельную ошибку точки входа Ring-3; она исправлена в v65.2. См. `VERIFICATION_V65_2_RU.md`.
