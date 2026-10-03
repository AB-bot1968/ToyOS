# Проверка Toy OS v65.3

## Комплектация

Проверено наличие в корне проекта:

- `build.sh`
- `build.bat`
- `src/vgadrv.c`
- `resources/SPLASH.RAW`
- `check78.sh`
- `check79.sh`

## Проверка сценариев

- `sh -n build.sh` — PASS.
- `./build.sh` находится и запускается; в локальной Linux x86_64 среде он
  доходит до собственной проверки и выдаёт ожидаемое требование i686 W64DevKit.
- `check78.sh` — PASS.
- `check79.sh` — PASS.
- `check80.sh` — PASS.

Полный build/runtime Windows 7 + x86 W64DevKit + QEMU должен дополнительно
быть выполнен в целевой среде пользователя.
