# ToyOS v67.11-B FIX60F — UART IRQ bounding / TESTLOAD isolation

Основа: FIX60E. FIX60E отклонён по реальному QEMU+com0com+SONARSIM тесту: первый Modbus request/response происходил, после чего VM зависала; TESTLOAD также давал FAIL=1.

FIX60F меняет физический IRQ4: один вход ISR обслуживает ровно одну причину IIR, RX/TX ограничены UART_IRQ_BURST=8. ISR не пытается удерживать Ring0 до полного успокоения меняющегося IIR. EOI выполняется общим interrupt_dispatch; оставшаяся работа обслуживается следующим IRQ4. PIT/FAT/console/protocol в IRQ отсутствуют.

TESTLOAD.TST теперь сам устанавливает baseline: mtstop ALL, rtstat stop/reset, hwwd disarm, safemode normal 0, затем проверяет нулевые RT/MT/handles/SAFE/HWWD до нагрузки. Добавлен TESTIRQ.TST и static checker check_fix60f_uart_irq.sh. Syscall 13/14 и политика I/O портов не менялись.

Приёмка: TESTLOAD с первого запуска FAIL=0; TESTIRQ/TESTUART/TESTSON/TESTSIM/TESTEXE FAIL=0; затем физический SONARSIM. QEMU/ToyOS должны оставаться отзывчивыми, request/response должны продолжать увеличиваться.
