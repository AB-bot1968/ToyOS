# Проверка FIX43

1. Собрать штатным `build.sh` в W64DevKit и загрузить QEMU.
2. Выполнить `test TESTBOOT.TST`. Ожидается `FAIL=0 PASSED`.
3. Выполнить полную регрессию: TESTLOAD, TESTKEY, TESTEVT, TESTLOG, TESTBOOT, TESTHB, TESTWD, TESTSUP, TESTRES, TESTPROC, TESTCORE. Все FAIL=0.
4. После регрессии без ручного cleanup выполнить `execmt MT01.EXE`; сообщения session active быть не должно. Затем `mtstop all`.
5. Ручная межзагрузочная проверка: `bootdiag mark 1`, затем перезапустить VM обычным внешним reset/reboot. При следующем входе в shell до prompt ожидается `BOOTDIAG REASON=1 ...`. `bootdiag` должен показать тот же marker. После проверки выполнить `bootdiag clear`.

Важно: FIX43 не пытается угадать причину внезапного power loss/reset без предварительно записанного marker. Аппаратный watchdog/reset будет отдельным этапом.
