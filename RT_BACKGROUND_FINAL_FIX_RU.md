# ToyOS RT background fix — final

## Исправления

- Удалён параметр `VERBOSE`/`Q` из запуска RTD: SENSOR1..SENSOR4 всегда работают
  как фоновые задачи и не имеют console-output режима.
- SENSOR1..SENSOR4 больше не используют `SYS_CONSOLE_WRITE` и `SYS_CONSOLE_POLL`.
  Измерения остаются kernel-side через `SYS_RT_STATS`.
- `rtstat watch` снова обновляет экран **раз в 1 секунду**: 100 RT ticks при
  RT clock 100 Hz.
- Устранён накопительный дрейф окна `watch`: следующий refresh планируется от
  предыдущей контрольной точки (`last_rt += 100`), а не от момента вывода.
- F10 больше не добавляет скрытый `q` к RTD-аргументам.
- Syscall ABI 1..48, FAT16 LBA 512 и регистронезависимый shell parser не меняются.
- Добавлена `check_rt_background_final.sh` для регрессии интерфейса RTD/SENSOR/watch.

## Контракт

```text
exec RTD.EXE SENSOR1.EXE PERIOD_MS DEADLINE_MS PRIORITY [DELAY_MS]
```

`VERBOSE` и `Q` больше не являются допустимыми параметрами.
