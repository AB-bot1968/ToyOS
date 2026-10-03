# Проверка ToyOS v67.11-B FIX57

1. Собрать `build.sh` в W64DevKit.
2. Загрузить образ в QEMU.
3. Выполнить `test TESTMB.TST`; требуется FAIL=0.
4. Выполнить `test TESTUART.TST`, `test TESTDATA.TST`, `test TESTINF.TST` и полный regression suite FIX56; везде FAIL=0.

TESTMB проверяет CRC known vector, F04 request, normal parse, partial frame, bad CRC, exception response и bounded timeout/retry.
Внешний COM не требуется: тестируется production Ring3 module, а не kernel test implementation.
Architecture Vision обновлён только для фиксации слоя Ring3 Modbus; миссия и syscall 13/14 не изменены.
