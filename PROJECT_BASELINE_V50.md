# Project baseline — v50

v50 продолжает v49. Исправлен конкретный дефект EXECMT: `image_end` загруженного
EXE стирался вызовом `sched_load_initial()`, из-за чего первый Ring-3
`SYS_CONSOLE_WRITE` не проходил `user_range()`.

Исправление минимальное: `execmt_load_one()` возвращает `image_end`, а
`execmt_prepare()` записывает его после `sched_load_initial()`.

Остальные функции и интерфейсы системы не изменены.
