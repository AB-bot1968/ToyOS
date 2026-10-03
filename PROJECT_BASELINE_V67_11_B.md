# ToyOS Project Baseline v67.11-B

Базовая точка: **v67.11-A FILESYSTEM OFFSET**.

## Файловая система

- FAT16 BPB расположен на LBA 512.
- Область до LBA 512 зарезервирована согласно v67.11-A.

## RT background

- SENSOR1..SENSOR4 могут работать detached от shell.
- F10 запускает подготовленную очередь RT-задач.
- `rtstat stop` останавливает все RT-задачи.
- `rtstat stop sensorN` останавливает только SENSORN.
- `rtstat watch` ESC останавливает только watch.

## ABI

Существующие syscall 1..48 сохранены. Для stop используется расширение существующего `SYS_RT_STATUS=43`.

## Следующий этап

v67.11-C: RTDATA, без изменения syscall 1..48; новый syscall/ABI будет добавлен только после отдельной фиксации спецификации.
