# Project baseline — v45

v45 продолжает v44 без изменения ABI, планировщика, EXEC/EXECQ, FAT16, shell и команды `ls`.

Изменение v45: начальная пользовательская строка `MTxx.EXE` теперь выводится как одна атомарная на уровне syscall строка `Task NN\n`, чтобы переключение задач не могло перемешать части сообщения.

Сохраняется диагностическая строка ядра `[MTxx] RUNNING ...`.

База: v44.

## V46 follow-up

V46 fixes console output atomicity and EXE/scheduler return critical sections. See `VERIFICATION_V46_RU.md`.
