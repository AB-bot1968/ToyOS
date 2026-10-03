# ToyOS v67.11-B FIX60Q — QEMU socket boundary / deterministic RX probe

## Подтвержденная ошибка FIX60P

`TESTRXP.TST` после PASS line 9 зависал на `ASSERT MT_PROGRESS 1 3`.
Причина тестовой архитектуры: автоматический TST запускал production `UARTRX.EXE`,
который включает реальный QEMU host serial backend. Такой тест не является
детерминированным и может повесить не ToyOS, а Windows COM chardev QEMU.

Предыдущий эксперимент уже показал: с `-chardev file` (TX-only) тот же длительный
SONARDRV и shell работают; зависание появляется при inbound traffic через
Windows COM/com0com. Это согласуется с опубликованными QEMU Windows serial
backend deadlock reports. Поэтому COM1/com0com больше не используется как
обязательный acceptance transport.

## Исправления

1. `UARTRX.EXE` остается только ручным physical probe.
2. `UARTRXT.EXE` собирается из того же `src/uartrx.c` с
   `UART_RX_TEST_INJECT=1`: physical transport выключен, 8 TX bytes потребляются
   op6, 17 RX bytes вводятся op5. Modbus/Data Channel по-прежнему отсутствуют.
3. `TESTRXP.TST` запускает только UARTRXT и проверяет MT progress, >=10 heartbeat,
   UART_ENABLED=0, IRQ4=0 и полную очистку состояния.
4. SONARSIM получает режим `--tcp HOST PORT` на Winsock. Все read/write waits
   bounded. COM mode сохранен для совместимости, но не рекомендуется для
   физической приемки на проблемном Windows QEMU.
5. Guest kernel, SONARDRV и Modbus wire contract не меняются.

## Автоматическая приемка

    test TESTUART.TST
    test TESTSONP.TST
    test TESTRXP.TST
    test TESTDATA.TST
    test TESTMB.TST
    test TESTSON.TST
    test TESTSIM.TST

Все FAIL=0. TESTRXP не требует SONARSIM/com0com/TCP.

## Физическая приемка через TCP chardev

QEMU 5.2-compatible server syntax:

    qemu-system-i386.exe -drive format=raw,file=build\toy_os.img -m 16M ^
      -chardev socket,id=sonar,host=127.0.0.1,port=4555,server,nowait,nodelay ^
      -device isa-serial,chardev=sonar,iobase=0x3f8,irq=4

После появления ToyOS запустить SONARSIM как TCP client:

    SONARSIM.EXE --tcp 127.0.0.1 4555 --slave 1 --mode normal ^
      --xyz 123456 -654321 2000000000

Ожидаем startup:

    SONARSIM: TCP 127.0.0.1:4555 ... READY bounded-socket-io

Затем ToyOS:

    execmt UARTRX.EXE

При устойчивой работе остановить `mtstop ALL`, `uartreset`, затем:

    execmt SONARDRV.EXE

SONARSIM должен непрерывно печатать `request=N ... response=17 tx=17`, shell
ToyOS должен оставаться доступным. `uartstat` должен показывать растущие HWRX/HWTX,
IRQ4=0 и отсутствие line errors/overrun.

## Архитектура

Architecture Vision обновлен: автоматические тесты не входят во внешний host
serial backend; TCP chardev разрешен как физический стендовый транспорт. Syscall
13/14, syscall 70, Ring3 policy, IER=0 и masked IRQ4 не изменены.

FIX60Q остается candidate до W64DevKit/QEMU runtime acceptance.
