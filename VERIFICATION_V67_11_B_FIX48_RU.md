# Проверка FIX48

1. Собрать штатным build.sh в W64DevKit.
2. Загрузить ToyOS в QEMU.
3. Выполнить `test TESTSFP.TST`; ожидается FAIL=0.
4. Выполнить полную регрессию всех TST.
5. После тестов без ручной очистки: `safemode`, `safepolicy`, `health`, затем `execmt MT01.EXE`. Состояние должно быть NORMAL, policy FLAGS=0, счётчики deny=0, новый MT session должен запускаться.

Ручная проверка: `safemode safe 4802`, затем `spawn EXIT0.EXE` и `execmt MT01.EXE` должны сообщить DENIED by SAFE policy; `safepolicy` должен увеличить соответствующие счётчики. После `safemode normal 0` запуск снова разрешён.
