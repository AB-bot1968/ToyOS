# ToyOS v67.11-B FIX60E — UART RX hardening / test isolation

## Причина
Физический тракт QEMU COM1 -> com0com -> SONARSIM доказал передачу запроса: SONARSIM получил request=1 и сформировал response=17. При первом RX-ответе VM зависала. Аудит выявил нарушение IRQ-дисциплины FIX55: IRQ4 вызывал `serial_time_us()`, который latch/read обращается к PIT channel 0, при входе IRQ и для каждого RX-байта. PIT0 одновременно является системным 100 Hz timebase/scheduler source.

## Исправление
IRQ4 больше не обращается к PIT. IRQ timestamps берутся из уже накопленного `rt_time_ticks` (10 ms resolution). Высокоточный `serial_time_us()` сохранён для Ring3 syscall 70/op4. Добавлен syscall 70/op8 — bounded RX flush (только tail=head), используемый SONARDRV перед каждой Modbus попыткой для исключения stale RX между retry.

## Тесты
- TESTUART.TST — прежняя детерминированная проверка transport.
- TESTSON.TST / TESTSIM.TST — parser/driver contract.
- TESTKEY.TST теперь имеет обязательный clean-state preamble.
- TESTSEQ.TST — отдельная проверка clean-state после нагрузочного теста; запускать сразу после TESTLOAD.TST.
- check_fix60e_uart_rx.sh запрещает PIT high-resolution calls из UART IRQ path и проверяет сохранение syscall 13/14.

## Приёмка
1. `test TESTLOAD.TST`
2. `test TESTSEQ.TST`
3. `test TESTKEY.TST` — FAIL=0 с первого запуска.
4. `test TESTUART.TST`, `test TESTSON.TST`, `test TESTSIM.TST`, `test TESTEXE.TST`.
5. Полный regression suite.
6. Physical: QEMU `-serial COM1`, SONARSIM COM2, `execmt SONARDRV.EXE`. VM не должна зависать; request/response должны продолжать увеличиваться.

FIX60E не считать stable до прохождения runtime/physical acceptance.
