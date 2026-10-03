# ToyOS v67.11-B FIX60H — проверка

Цель: исключить IRQ4 из физического SONAR transport после подтверждённого зависания на повторной транзакции при THRE IRQ.

Изменения:
- RX и TX syscall 70 обслуживаются bounded polling;
- UART IER остаётся 0 в physical transport mode;
- SONARDRV использует op10 (100-Hz monotonic microsecond epoch) для timeout;
- TESTPHY.TST проверяет transport policy и чистое состояние;
- syscall 13/14 и политика I/O ports не изменены.

Перед физическим тестом: TESTLOAD, TESTIRQ, TESTUART, TESTSON, TESTSIM, TESTEXE, TESTPHY должны иметь FAIL=0.
Физический критерий: execmt SONARDRV.EXE возвращает shell; QEMU остаётся жив; SONARSIM request/response растут более двух циклов.
Architecture Vision обновлён разделом FIX60H.
