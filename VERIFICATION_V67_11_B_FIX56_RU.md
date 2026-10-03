# Проверка ToyOS v67.11-B FIX56

Статус: CANDIDATE до runtime-проверки W64DevKit/QEMU.

Обязательный новый тест:
`test TESTDATA.TST`
Ожидание: FAIL=0.

После него выполнить `test TESTUART.TST`, `test TESTINF.TST` и полный regression suite FIX55A.

TESTDATA проверяет: begin generation, два независимых reader, publish/read metadata+payload, EMPTY, bounded overwrite/OVERRUN и счётчик, oldest available sample, смену generation, продолжение чтения после restart, bad user pointer, close/reset и чистое состояние системы.

Architecture Vision обновлён: добавлен фактически реализованный FIX56 Data Channel. Миссия и политика I/O syscall 13/14 не менялись.
