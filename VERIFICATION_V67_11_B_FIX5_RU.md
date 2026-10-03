# v67.11-B-FIX5 — RTSTAT / BACKGROUND RT

## Исправления

1. `rtstat stop SLOT1..SLOT4` адресует RT slot напрямую. `SLOT1` = внутренний slot 0, `SLOT4` = внутренний slot 3. Имена SENSOR в STOP больше не принимаются. Сравнение параметров выполняется без учёта ASCII-регистра.
2. F10 теперь выводит имя реально запущенной queued-задачи (`SENSOR1.EXE`, `SENSOR3.EXE` и т.д.), а не безличный номер `task N`. Это устраняет неоднозначное финальное информационное сообщение при пакетном запуске.
3. `rtstat watch` имеет жёсткий период обновления 100 RT ticks (= 1 s) и выводит текущий RT tick в заголовке каждого обновления. Это позволяет однозначно видеть, что монитор продолжает получать новые snapshots.
4. Kernel RT statistics/ring не менялись по ABI: completed-job samples по-прежнему записываются через `rt_stats_record()`. Нулевые `dispatch/cpu/wall/jitter` для сверхкороткой SENSOR job могут быть корректным результатом дискретности 100-Hz RT clock; они не подменяются искусственными единицами.

## Проверка

- `check_rt_background_fix5.sh`: PASS
- i386-compatible freestanding compilation `kernel.c`: PASS
- i386-compatible freestanding compilation `user_shell.c`: PASS
- i386-compatible freestanding compilation `rtd.c`: PASS
- i386-compatible freestanding compilation `rt_sensor_diag.c`: PASS

Полный `build.sh` в Linux-среде не объявляется PASS, потому что проект требует W64DevKit/i686 target.
