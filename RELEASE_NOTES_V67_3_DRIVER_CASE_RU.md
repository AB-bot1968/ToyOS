# ToyOS v67.3 — Driver Parameter Case Fix

## Исправление

Сделаны регистронезависимыми текстовые параметры пользовательских драйверов:

- COMDRV: `COM1..COM4`, `SEND`, `RECV`;
- NETDRV: `TELEMETRY`, `MASTER`, `SLAVE1`, `SLAVE2`, `SEND`, `RECV`;
- LOADER: `RECV`;
- VGADRV: имя файла ресурса перед открытием нормализуется в upper-case.

## Совместимость

Изменений syscall ABI, EXE1 ABI и FAT16 не выполнялось.

## Проверка

`check129.sh` проверяет наличие нормализации во всех четырёх драйверах и компилирует их с теми же freestanding-флагами проекта.
