# ToyOS v67.11-B FIX60O — проверка

## Подтверждённые причины
- FIX60N снова разрешал scheduler сразу после `SYS_EXECMT`, поэтому первый IRQ0 мог отнять foreground continuation до строки `started`/prompt. Наблюдаемая последняя строка `loading 1 independent tasks...` соответствует этой гонке.
- В FIX60N SONARSIM печатал `response=17` до Windows `WriteFile`; эта строка доказывала построение ответа, но не успешную передачу 17 байт в COM.
- TESTSONN проверял реальный MT progress только на no-peer/timeout path и не выполнял успешную ветку response->publish->heartbeat.

## FIX60O
- EXECMT: PREPARED -> explicit state-only commit -> IRQ0 dispatch.
- Interactive commit происходит только в foreground input wait после prompt.
- TST `MT_PROGRESS` использует syscall 75 и затем проверяет реальные dispatch/cpu_ticks.
- TESTSONO/SONARTST покрывают successful response path, Data Channel и heartbeat.
- SONARSIM: overlapped RX/TX, bounded waits, cancel on timeout, truthful tx diagnostics.

## Автоматические проверки в исходном пакете
- `check_fix60o_execmt_serial_boundary.sh`
- `TESTMTN.TST`, `TESTSONN.TST`, новый `TESTSONO.TST`
- прежние TESTUART/TESTDATA/TESTMB/TESTSON/TESTSIM/TESTPHY и полный regression suite.

## Физическая приёмка
1. Запустить новый SONARSIM и дождаться `READY bounded-overlapped-io`.
2. Запустить QEMU/ToyOS и дождаться `toy0>`.
3. `execmt SONARDRV.EXE` обязан сначала вывести `execmt: started in background; quantum=20 ms`, затем новый `toy0>`.
4. SONARSIM должен показывать `request=N ... response=17 tx=17`; N должен расти. `tx=TIMEOUT` однозначно означает проблему Windows/com0com передачи, а не подтверждённый RX ToyOS.
5. Shell должен принимать команды параллельно. `mtstat`/`ps` должны оставаться доступными.

Architecture Vision изменён: зафиксирован двухфазный EXECMT commit и bounded host-serial acceptance boundary. Миссия ToyOS и I/O-port policy не изменены.

FIX60O — candidate до W64DevKit/QEMU/com0com acceptance пользователя.
