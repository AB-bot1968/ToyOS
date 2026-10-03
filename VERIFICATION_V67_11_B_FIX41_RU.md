# Проверка FIX41

## 1. Новая проверка event log
После чистой загрузки:

    test TESTEVT.TST

Ожидается итог `FAIL=0 PASSED`. Тест очищает журнал, запускает FAULTUD.EXE через TYPE/KEY ENTER, ждёт PID и структурированно проверяет reason=FAULT в kernel event ring.

Затем:

    eventlog

Должна присутствовать запись fault с ненулевым PID и REASON=2, VECTOR=6.

    eventlog clear
    eventlog

После clear таблица должна быть пустой.

## 2. Cleanup нагрузочных тестов

    test TESTLOAD.TST
    rtstat

и отдельно после перезагрузки:

    test TESTKEY.TST
    rtstat

Оба TST теперь после `rtstat stop` выполняют `rtstat reset`. После PASS не должно требоваться ручное `rtstat reset`. MT session также закрывается `mtstop ALL`; `ASSERT MT_ACTIVE 0`, `ASSERT RT_ACTIVE 0`, `ASSERT PROC_HANDLES 0` остаются обязательными.

## 3. Полная регрессия

    test TESTLOAD.TST
    test TESTKEY.TST
    test TESTEVT.TST
    test TESTHB.TST
    test TESTWD.TST
    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST

Все тесты должны завершиться `FAIL=0`.

После этого вручную проверить 8 RT + MT, RTSTAT WATCH/ESC, MTSTAT WATCH/ESC, wait/ESC и spawn ARGVDIAG.EXE ONE TWO THREE.

## Важно
FIX41 пока хранит event log только в RAM. Запись FAT16 намеренно не выполняется из fault/exit critical path. Полная W64DevKit/QEMU runtime-проверка должна быть выполнена на целевой среде.

## FIX41A: исправление FAT 8.3
В FIX41A тест события переименован из недопустимого `TESTEVENT.TST` в `TESTEVT.TST`.
Запускать: `test TESTEVT.TST`.
Это только исправление сборки/имени файла; логика event log не изменена.
