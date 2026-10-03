# Проверка ToyOS v67.11-B FIX42

База: подтвержденный stable FIX41B. FIX42 до проверки в W64DevKit/QEMU является candidate.

## 1. Обязательный автоматический тест
После чистой загрузки:

    test TESTLOG.TST

Ожидается `FAIL=0` и `PASSED`. Тест очищает RAM event log, создаёт контролируемый #UD fault, сохраняет журнал в EVENT.LOG, очищает RAM, проверяет количество и REASON непосредственно из бинарного файла, выводит disk snapshot, удаляет EVENT.LOG и проверяет его отсутствие. В конце закрывается MT-session и проверяются MT_ACTIVE=0, RT_ACTIVE=0, PROC_HANDLES=0.

## 2. Ручная проверка разделения RAM/disk

    eventlog clear
    spawn FAULTUD.EXE
    wait <PID>
    eventlog save
    eventlog clear
    eventlog
    eventlog disk

RAM-таблица после clear должна быть пустой, а `eventlog disk` должен показывать сохранённый fault. Затем:

    eventlog clear disk
    eventlog disk

Ожидается `eventlog: disk missing/invalid`.

## 3. Полная регрессия

    test TESTLOAD.TST
    test TESTKEY.TST
    test TESTEVT.TST
    test TESTLOG.TST
    test TESTHB.TST
    test TESTWD.TST
    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST

После неё без ручного cleanup проверить `execmt MT01.EXE`; сообщение `session active` появляться не должно.
