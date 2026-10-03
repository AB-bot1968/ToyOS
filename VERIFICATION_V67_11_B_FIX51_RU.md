# ToyOS v67.11-B FIX51 — Managed Process Recovery Lifecycle

База: FIX50 Recovery Manager CURRENT STABLE.

## Что изменено
- SYS_RECOVERY_MANAGER остаётся syscall 68; операции FIX50 op=0..2 сохранены.
- Добавлены lifecycle-состояния managed recovery и расширенный read-only query op=7.
- Supervisor сообщает Recovery Manager этапы STOPPED, SPAWNED, VERIFIED и FAILED.
- Recovery успешен только после heartbeat нового PID.
- Restart budget остаётся равным 3; исчерпание бюджета переводит систему в SAFE.
- Добавлен RCONCE.EXE — детерминированный test-only worker: первый экземпляр создаёт marker, намеренно оставляет PID-owned handle и вызывает #UD; новый экземпляр того же EXE выдаёт heartbeat, удаляет marker и нормально завершается.
- Портовый I/O, RT/MT scheduler policy, FAT16 critical-path policy не изменены.

## Architecture Vision
TOYOS_ARCHITECTURE_VISION.md обновлён: зафиксировано, что recovery считается успешным только после containment/cleanup старого экземпляра, создания нового PID и подтверждённого heartbeat нового экземпляра.

## Основной автоматический тест FIX51
В ToyOS shell:

    test TESTRST.TST

Ожидается итог PASS без FAIL. Тест автоматически доказывает:
1. fault первого RCONCE;
2. освобождение намеренно оставленного process-owned FAT handle;
3. bounded решение RESTART;
4. создание отличающегося нового PID;
5. heartbeat нового экземпляра и состояние RECOVERED;
6. failure path на HANGWD с тремя restart-разрешениями;
7. исчерпание budget и SAFE;
8. финальную очистку NORMAL / MT=0 / RT=0 / handles=0 / HWWD=off / health=NORMAL.

## Полная регрессия
После TESTRST выполнить:

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
    test TESTRCM.TST
    test TESTRST.TST

После workload/длительного прогона повторить минимум TESTCORE, TESTPROC, TESTRES, TESTRCM и TESTRST.

## Host/static check

    ./check_fix51_managed_recovery.sh

Runtime PASS в QEMU/W64DevKit остаётся обязательным условием фиксации FIX51 как CURRENT STABLE.
