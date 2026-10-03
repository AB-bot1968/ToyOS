# ToyOS Project Baseline v67.11-B-FIX1

База проекта: v67.11-B + исправление фонового RT console race.

Сохраняется FAT16 start LBA 512. Сохраняются syscall 1..48 и существующий RT scheduler. F10 запускает SENSOR1..SENSOR4 в фоне без асинхронного console output. `rtstat` остаётся доступным из shell. `rtstat stop` и `rtstat stop sensorN` сохраняются.
