# Проверка ToyOS v67.11-B FIX41B

## Причина исправления
После успешной полной регрессии один из spawn/supervisor тестов мог оставить пустую, но формально открытую MT-сессию. `ASSERT MT_ACTIVE 0` проверяет отсутствие живых MT-задач, но не закрывает саму session. Поэтому следующий ручной `execmt MT01.EXE` корректно отвечал `session active`.

## Исправление
TESTHB.TST, TESTWD.TST, TESTSUP.TST и TESTRES.TST теперь в конце выполняют `RUN mtstop ALL`. TESTKEY.TST, TESTLOAD.TST и TESTEVT.TST уже имели явный cleanup. Политика scheduler не изменялась.

## Основная проверка
После чистой загрузки выполнить подряд:

    test TESTLOAD.TST
    test TESTKEY.TST
    test TESTEVT.TST
    test TESTHB.TST
    test TESTWD.TST
    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST

Все тесты должны завершиться без FAIL. Сразу после этого, БЕЗ ручного `mtstop all`, выполнить:

    execmt MT01.EXE
    mtlist
    mtstop all

`execmt` обязан создать новую MT-сессию и не должен выводить `session active`.

Затем проверить `rtstat`: TESTLOAD/TESTKEY должны оставлять RT остановленными и статистику reset. При необходимости повторить обычные RTSTAT WATCH/ESC, MTSTAT WATCH/ESC и eventlog.

## Критерий провала
Любой `session active` после полностью успешной регрессии, зависание, необходимость второго Enter, изменение RT MISS/SKIP поведения или регрессия eventlog означает, что FIX41B не фиксируется stable.
