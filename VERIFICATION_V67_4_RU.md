# ToyOS v67.4 — Verification

## Статические проверки

PASS:
- AUTOSTART использует `/SPLASH.RAW`;
- VGADRV формирует абсолютный путь ресурса;
- VGADRV сохраняет fallback на исходное нормализованное имя;
- `fail_and_exit()` восстанавливает text mode до `SYS_EXIT`;
- `SYS_VIDEO_TEXT` очищает B8000 и ставит cursor `(0,0)`;
- `SPLASH.RAW` имеет размер 64000 байт.

## Ограничение среды

Полная `build.sh` не запускалась в данной среде: проектный build требует
предусмотренный ToyOS i686/W64DevKit toolchain. Изменённые исходники проверяются
отдельно с freestanding-флагами проекта.

> v67.5: положение успешного сообщения `VGADRV: OK` исправлено отдельно; подробности в `VGADRV_BOTTOM_PROMPT_FIX_V67_5_RU.md`.
