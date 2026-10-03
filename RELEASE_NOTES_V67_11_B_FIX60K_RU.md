# ToyOS v67.11-B FIX60K — atomic foreground return before EXECMT dispatch

## Исправленная ошибка
FIX60J всё ещё разрешал `sched_active=1` внутри `SYS_EXECMT`. Поэтому IRQ0 мог запустить первую independent-задачу сразу после возврата Ring3, до того как shell успевал завершить команду, вывести `execmt: started...`, приглашение и войти в устойчивый `SYS_CONSOLE_READ`. Для постоянно работающего SONARDRV это создавало гонку владения foreground continuation; наблюдаемый симптом — последняя строка `execmt: loading 1 independent tasks...`, затем зависание.

## FIX60K
`execmt_prepare()` теперь только валидирует/загружает EXE1 и создаёт READY-задачи. Сессия помечается `mt_session_active`, но scheduler остаётся неактивным. Первый dispatch разрешается только из foreground `SYS_CONSOLE_READ`, то есть после полного возврата `SYS_EXECMT`, сообщения shell и приглашения `toy0>`.

SONARDRV остаётся detached/background и не пишет в console. FIX60I hardware+software RX flush и FIX60J cooperative yield сохранены.

## Автоматические ToyOS-тесты
- `TESTMTK.TST`: EXECMT HBEATOK -> MTSTAT/PS -> MTSTOP; проверяет, что foreground продолжает исполняться.
- `TESTSONK.TST`: реальный `/SONARDRV.EXE` -> MTSTAT/PS -> MTSTOP; проверяет, что запуск SONARDRV не крадёт shell даже без физического COM peer.

## Host checks
`check_fix60k_execmt_commit.sh`, а также FIX58..FIX60J checks должны завершаться PASS.
