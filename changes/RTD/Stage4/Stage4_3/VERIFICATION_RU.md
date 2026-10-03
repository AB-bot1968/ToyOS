# Проверка Stage 4.3

## Автоматические проверки

- `src/rt_sensor_diag.c` существует: PASS
- два compile-time варианта диагностического приложения: PASS
- различимый вывод SENSOR1: PASS
- различимый вывод SENSOR2: PASS
- ESC для SENSOR1/SENSOR2: PASS
- сборка SENSOR1.EXE: PASS
- сборка SENSOR2.EXE: PASS
- FAT16-установка SENSOR1.EXE: PASS
- FAT16-установка SENSOR2.EXE: PASS
- `rt_pick_ready()`/priority arbitration остаются в коде Stage 4.2: PASS
- Stage 4.3 host test: PASS

Runtime-проверка зависит от фактического запуска ToyOS в целевой среде.
