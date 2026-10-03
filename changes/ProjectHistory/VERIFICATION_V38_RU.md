# Проверка v38 corrected — scheduler lifecycle

## Цель
Проверяется переход от фиксированного A/B эксперимента v37 к четырём независимым Ring-3 задачам с жизненным циклом `CREATE -> READY -> RUNNING -> BLOCKED/STOPPED -> EXIT`.

## Структурные проверки
`check30.sh` … `check37.sh` сохраняются. `check38.sh` проверяет четыре TCB/CR3, состояния CREATE/READY/RUNNING/BLOCKED/STOPPED/EXIT, round-robin READY, syscall 20 BLOCK, syscall 21 WAKE, syscall 22 EXIT, задачи A/B/C/D, сохранение v36 memory-isolation и неизменность 19-словного `struct frame`.

## Поведенческий тест QEMU
После `mt-test` ожидаются события A BLOCKED, B видит отсутствие A marker и будит A, B BLOCKED, C будит B и EXIT, D EXIT, затем A возобновляется и подтверждает сохранность marker, после чего A и B EXIT и shell получает управление.

## Ограничение
Полный PE/COFF runtime W64DevKit + Windows 7 + QEMU в текущей среде не выполнялся; локальные проверки не заменяют запуск на целевой машине.
