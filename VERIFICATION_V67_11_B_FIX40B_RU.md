# Проверка FIX40B

База — подтвержденный FIX40A. Сначала выполнить:

    test TESTKEY.TST

Ожидается FAIL=0. Тест автоматически: поднимает 4 MT + 8 RT через TYPE/KEY ENTER/KEY F10; проверяет и останавливает нагрузку; запускает `spawn ARGVDIAG.EXE ONE TWO THREE`; ожидает NORMAL/status 0; проверяет точный MTDATA `ARGV:ARGVDIAG.EXE|ONE|TWO|THREE`; затем последовательно запускает FAULTUD/FAULTGP/FAULTPF, ожидая reason FAULT; завершает MT session и проверяет MT_ACTIVE=0, RT_ACTIVE=0, PROC_HANDLES=0.

После него выполнить регрессию:

    test TESTLOAD.TST
    test TESTHB.TST
    test TESTWD.TST
    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST

Все тесты должны иметь FAIL=0. Затем вручную проверить RTSTAT WATCH/ESC и MTSTAT WATCH/ESC.

FIX40A остается точкой отката до runtime-подтверждения FIX40B.
