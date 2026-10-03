# ToyOS v67.11-B FIX60P — UART RX isolation / endurance

## Основание

Физический эксперимент FIX60O дал важное разделение: с QEMU `-chardev file`
`execmt SONARDRV.EXE` длительно работает, возвращает shell и непрерывно пишет TX,
но с Windows COM1 <-> com0com <-> COM2 и входящими ответами система зависает
после нескольких обменов. Следовательно, EXECMT и длительный TX сами по себе
не являются достаточной причиной зависания; требуется изолировать physical RX.

Также обнаружена ошибка TESTUART: deterministic op6 TX-drain выполнялся без
явного выключения physical transport. При включённом transport op2 может уже
передать байты в UART, поэтому старое ожидание `TX DRAIN=200` ложно.

## Изменения FIX60P

1. TESTUART перед deterministic hooks выполняет op7=0 и reset. Test hooks не
   конкурируют с physical polling.
2. op11 syscall 70 возвращает 16 DWORD bounded UART diagnostics без PIT latch.
   Команда `uartstat` выводит HWRX/HWTX, poll counters, RX high-water, flush,
   OE/PE/FE/BI, software overrun и unexpected IRQ4.
3. SONARTST (тот же sonardrv.c с SONAR_TEST_INJECT) работает offline: TX
   удаляется через op6, response вводится op5. Host COM не участвует.
4. TESTSONP.TST требует 1000 successful response->parse->decode->publish->heartbeat
   циклов с bounded wait.
5. UARTRX.EXE — физический probe. Он отправляет валидный F04 request и только
   принимает 17 байт. Нет response parser, XYZ decode, Data Channel и console.
6. TESTRXP.TST проверяет, что UARTRX запускается как MT, получает CPU и IRQ4
   остаётся равен нулю без peer.

## Порядок автоматической приёмки

Сначала без SONARSIM/com0com peer:

    test TESTUART.TST
    test TESTSONP.TST
    test TESTRXP.TST
    test TESTDATA.TST
    test TESTMB.TST
    test TESTSON.TST
    test TESTSIM.TST

Все тесты должны иметь FAIL=0. TESTSONP обязан пройти 1000 публикаций/heartbeat.

## Физическая локализация

Обязательно пересобрать `tools\sonarsim\SONARSIM.EXE` из этого пакета и
убедиться, что startup содержит `READY bounded-overlapped-io`, а каждая
успешная запись — `tx=17`.

A. Сначала SONARSIM READY, затем QEMU `-serial COM1`, затем:

    execmt UARTRX.EXE
    uartstat

При исправном physical RX request должен расти, heartbeat UARTRX — тоже;
`uartstat` должен показывать растущий HWRX/HWTX, IRQ4=0, без OE/PE/FE/BI. После остановки probe команда `uartreset` принудительно выключает transport и очищает кольца/диагностику.
Если здесь возникает hang, SONAR parser/Data Channel исключены из причины.

B. Только если UARTRX стабилен:

    mtstop ALL
    execmt SONARDRV.EXE
    uartstat

Если UARTRX стабилен, а SONARDRV нет — искать в successful protocol/data path.
Если оба зависают только с com0com, сравнить с уже подтверждённым `-chardev file`:
это локализует дефект на physical RX / QEMU Windows serial backend boundary.

Порядок запуска предпочтительный: SONARSIM READY -> QEMU/ToyOS -> probe/driver.
Отсутствие peer не должно блокировать ToyOS: timeout/retry bounded.

Architecture Vision обновлён. Syscalls 13/14 не изменены. FIX60P остаётся
candidate до W64DevKit/QEMU физической приёмки.
