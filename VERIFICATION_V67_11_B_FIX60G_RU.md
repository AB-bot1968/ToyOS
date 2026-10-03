# ToyOS v67.11-B FIX60G — UART RX POLL

Основа: FIX60F. Все первичные тесты FIX60F, включая TESTLOAD, прошли на W64DevKit/QEMU, но реальный SONARDRV по-прежнему приводил к hard hang всей VM сразу после первого 17-байтового ответа SONARSIM.

FIX60G устраняет непокрытый детерминированными TESTUART/TESTIRQ путь: физический RX IRQ4. При включении transport RX interrupt 16550 больше не разрешается. SYS_UART_TRANSPORT op1 перед чтением kernel ring ограниченно (UART_POLL_BURST=32) дренирует hardware RX FIFO. TX остаётся IRQ-driven через THRE. IRQ4, если возникает из TX, остаётся bounded FIX60F. ABI syscall 70 сохранён; op9 добавлен только как диагностическое чтение IER. Syscall 13/14 и принятая политика Ring3 I/O не менялись.

Приёмка: сначала TESTLOAD, TESTIRQ, TESTUART, TESTSON, TESTSIM, TESTEXE. Затем физический SONARSIM + `execmt SONARDRV.EXE`. Требуется: shell/QEMU не зависают; request/response растут многократно. После этого полный regression.
