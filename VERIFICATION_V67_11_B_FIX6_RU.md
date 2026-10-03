# ToyOS v67.11-B FIX6 — RT background / F10 / RTSTAT WATCH

Исправления текущего релиза без перехода к RTDATA.

## Исправлено

1. SENSOR1–4 больше не выполняют `SYS_CONSOLE_POLL`.
   Фоновая RT-задача полностью отделена от клавиатуры shell. `rtstat stop SLOTn`
   останавливает задачу непосредственно через kernel RT slot.

2. Сообщение успешного запуска при F10 теперь формирует shell после успешного
   `SYS_EXEC_ARGS`. RTD больше не печатает сообщение об успехе сам. Это устраняет
   гонку/перемешивание строк, когда следующий RT launch мог быть запущен до
   завершения предыдущего вывода.

3. В сообщении F10 выводятся точные сохранённые аргументы конкретного pending
   запуска: имя SENSOR и period/deadline/priority/(delay). Регистр параметров
   shell остаётся нечувствительным благодаря существующему ASCII-fold parser.

4. `RTSTAT WATCH` сохраняет обновление один раз в секунду по RT clock 100 Hz.
   Snapshot выполняется после контрольного интервала, а не используется старый
   буфер предыдущего обновления.

5. Существующая статистика kernel ring и `RTSTAT DUMP` не изменялись по ABI.
   Последние completed jobs продолжают записываться через `SYS_RT_WAIT`.

## Проверки

- `check_rt_background_final.sh`: PASS
- изменённые `kernel.c`, `user_shell.c`, `rtd.c`, `rt_sensor.c`: i386-compatible
  freestanding compile: PASS

Полная production-сборка ToyOS требует штатного W64DevKit/i686 toolchain.
