# VERIFICATION — ToyOS v67.11-B FIX50 Recovery Manager

База: подтвержденный FIX49 AUTOMATED ACCEPTANCE (CURRENT STABLE).

## Критерий приемки
Каждый автоматический TST ниже обязан завершиться с FAIL=0. Любой FAIL>0 означает, что FIX50 не фиксируется как stable.

## 1. Новый функциональный тест FIX50

    test TESTRCM.TST

Проверяет централизованный Recovery Manager: лимит 3 restart, передачу fault в manager, решения RESTART/SAFE, переход DEGRADED, исчерпание budget, recovery event chain, monotonic seq/tick, persistence EVENT.LOG и финальную очистку.

## 2. Полная регрессия

    test TESTLOAD.TST
    test TESTKEY.TST
    test TESTEVT.TST
    test TESTLOG.TST
    test TESTBOOT.TST
    test TESTSAFE.TST
    test TESTPOL.TST
    test TESTHWWD.TST
    test TESTHLTH.TST
    test TESTSFP.TST
    test TESTHB.TST
    test TESTWD.TST
    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST
    test TESTRCV.TST
    test TESTACC.TST

## 3. Повторная acceptance после всей нагрузки

    test TESTRCM.TST
    test TESTACC.TST
    test TESTCORE.TST
    test TESTRES.TST
    test TESTPROC.TST

Все пять повторных тестов: FAIL=0.

## Финальное состояние
TESTRCM/TESTACC автоматически требуют NORMAL, MT=0, RT=0, process handles=0, HWWD disarmed и HEALTH_STATE=0.

Ручное функциональное тестирование для приемки FIX50 не требуется, если весь этот набор проходит с FAIL=0.
