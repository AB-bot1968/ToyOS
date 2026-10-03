# ToyOS v67.9 — Project Baseline

База: **v67.8 Stage 6.2 RT Jitter**.

Добавлено: расширенная RT timing observability и четыре диагностические RT-задачи SENSOR1..SENSOR4.

Стабильные ограничения:

- freestanding 32-bit i386;
- no libc / no hosted runtime;
- существующие syscall 1..46 ABI unchanged;
- новый read-only diagnostic syscall 47;
- RT scheduler policy unchanged;
- RT clock 100 Hz / 10 ms tick;
- максимум четыре detached RT tasks;
- shell command input case-insensitive;
- изменения сопровождаются source comments, docs и executable check.
