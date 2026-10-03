# VERIFICATION v50

## EXECMT: исправлен реальный путь первого Ring-3 SYS_CONSOLE_WRITE

В v50 найден и устранён дефект, из-за которого `MT01.EXE ... MT04.EXE`
не могли вывести собственную строку `Task NN`.

### Точная причина

`execmt_load_one()` загружал EXE и записывал:

`sched_tasks[id].image_end = bss_end`

Но сразу после этого `execmt_prepare()` вызывал:

`sched_load_initial(&sched_tasks[i], ...)`

а `sched_load_initial()` начинается с полного `mem_set(t,0,sizeof(*t))`.

Таким образом, только что вычисленное `image_end` стиралось и становилось нулём.

При первом Ring-3 вызове:

`SYS_CONSOLE_WRITE(task_line, 8)`

обработчик `INT 80h` выполнял `user_range()`. Для EXECMT-задачи проверка:

`p >= EXEC_LOAD_ADDR && end <= sched_tasks[sched_current].image_end`

завершалась неуспешно, потому что `image_end == 0`.

В результате syscall возвращал `0xffffffff` и строка `Task NN` не выводилась. Это объясняет,
почему диагностическая строка планировщика присутствовала, а сообщение самого EXE отсутствовало.

### Исправление

`execmt_load_one()` теперь возвращает проверенную границу образа через отдельный
`image_end_out`.

После `sched_load_initial()` эта граница восстанавливается:

`sched_tasks[i].image_end = image_end`

Поэтому первый `SYS_CONSOLE_WRITE` из настоящего Ring-3 EXE проходит существующую
проверку `user_range()` и выводит строку.

### Почему это минимальное исправление

Не изменялись:

- INT 80h ABI;
- IDT/DPL syscall;
- ISR frame;
- `exec`;
- `execq`;
- `ls`;
- shell/menu;
- FAT16;
- CR3;
- физические image slots;
- физические стеки;
- планировщик;
- формат MTxx.EXE;
- диагностическая строка `[MTxx] RUNNING ...`.

Исправлен только жизненный цикл `image_end` для EXECMT-задач.

### Проверка Windows 7 + QEMU

Штатная сборка должна выполняться именно штатным x86 W64DevKit под Windows 7,
после чего `build.sh` формирует `build/toy_os.img`. Архив не заменяет эту
проверку эмулятором Linux: финальная функциональная проверка должна выполняться
в вашей Windows 7 + QEMU среде.

Для проверки:

`execmt mt01.exe mt02.exe mt03.exe mt04.exe`

Ожидается сохранение диагностических строк и появление строк самих программ:

`Task 01`
`Task 02`
`Task 03`
`Task 04`

с последующим штатным завершением без зависания.
