# ToyOS project baseline — v67.11-B-FIX2

База: **v67.11-A → v67.11-B → v67.11-B-FIX1 → v67.11-B-FIX2**.

Ключевые инварианты:
- 32-bit i386 freestanding OS;
- FAT16 начинается на LBA 512;
- kernel slot остаётся LBA 1..128;
- syscall ABI 1..48 не изменён;
- все команды shell регистронезависимы;
- F10 запускает подготовленные RT-задачи в фоне;
- SENSOR1..SENSOR4 по F10 не пишут периодический диагностический текст в интерактивную консоль;
- RTSTAT остаётся источником scheduler/timing statistics;
- RTDATA ещё не реализован.
