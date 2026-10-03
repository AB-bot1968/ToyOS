# Проверка ToyOS v67.11-B FIX46

1. Собрать штатным `build.sh` в требуемом i686 W64DevKit.
2. Загрузить образ в QEMU.
3. Выполнить `test TESTHWWD.TST`; ожидается PASS/FAIL=0.
4. Выполнить `hwwd`; ожидается BACKEND=1, ARMED=0 после теста.
5. Ручной цикл: `hwwd arm 100`, `hwwd feed`, `hwwd`, `hwwd disarm`, `hwwd`.
6. Выполнить полную регрессию TESTLOAD, TESTKEY, TESTEVT, TESTLOG, TESTBOOT, TESTSAFE, TESTPOL, TESTHWWD, TESTHB, TESTWD, TESTSUP, TESTRES, TESTPROC, TESTCORE.
7. Без ручной очистки выполнить `execmt MT01.EXE`; не должно быть `session active`.

Ограничение FIX46: backend 1 эмулирует только control-plane для регрессии и не является независимым аппаратным watchdog. Физический reset/timeout backend не реализован. PIT не выполняет feed.
