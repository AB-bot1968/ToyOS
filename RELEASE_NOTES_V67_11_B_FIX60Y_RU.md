# ToyOS v67.11-B FIX60Y — RT / SONAR acceptance fixes

Основа: FIX60X TEST CONSOLE SUMMARY. 43 исходных TEST*.TST из FIX60T не изменялись.

Исправления по фактической итоговой сводке `test`:

- RT: `SYS_RT_YIELD` теперь сначала обслуживает наступившие release RT-задач, затем выбирает READY job. `RT_SLOT_PROGRESS` ожидает реальный рост completed-job статистики в ограниченном окне вместо жёсткой привязки к 32 вызовам yield.
- SONARTST: heartbeat успешного цикла фиксируется до публикации Data Channel. Поэтому наблюдение N-й публикации больше не может опередить N-й heartbeat на один syscall.
- SONARPUB/SONARLOG: детерминированный producer для TESTLGR снижен с 50 Hz до 25 Hz. SONARLOG делает две FAT pwrite commit-операции на sample; новый темп оставляет consumer время обработать все записи и проверяет именно cyclic logger, а не искусственный Data Channel overrun.
- TEST console summary FIX60X сохранён. TST.LOG и RAM-буфер полного отчёта не возвращены.

Полная runtime-проверка должна выполняться штатным i686 W64DevKit/QEMU. В текущей среде доступна только host/static проверка, так как системный gcc имеет target x86_64.
