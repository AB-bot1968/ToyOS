# ToyOS v67.11-B FIX59 — SONARSIM

FIX59 добавляет внешний Windows 7/W64DevKit `SONARSIM.EXE` — детерминированный Modbus RTU slave для стендовой проверки `SONARDRV.EXE` из FIX58.

Поддержаны Function 04, slave 1 по умолчанию, 6 input registers для X/Y/Z signed int32 mm (high word first), динамические и фиксированные координаты. Fault injection: no-response, bad-crc, delay, partial, exception. Kernel ABI и syscall 13/14 не изменены.

Автоматические проверки: host `SONARSIM.EXE --selftest`, статический `check_fix59_sonarsim.sh`, ToyOS `TESTSIM.TST` плюс полный унаследованный regression suite.
