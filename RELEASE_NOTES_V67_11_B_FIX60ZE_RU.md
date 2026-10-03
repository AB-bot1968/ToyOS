# ToyOS v67.11-B FIX60ZE — SONARVWR как временная MT-задача

## Причина
В FIX60ZG/FIX60ZHD действует проверенное правило scheduler: пока активен обычный foreground EXE (`exe_active`), MT-задачи намеренно не диспетчеризуются. Поэтому запуск `exec SONARVWR.EXE` приостанавливал `SONARLOG.EXE` на всё время работы viewer. Уменьшение числа выводимых записей только сокращало паузу и не устраняло её.

Аудит FAT16 показал, что SONARLOG и SONARVWR получают независимые handles (собственные `pos/size/start/owner_pid`), а PREAD/PWRITE выполняются внутри одного syscall и не могут быть вытеснены MT scheduler посередине низкоуровневой FAT-операции. Телеметрический формат дополнительно использует два заголовка с generation для согласованного snapshot.

## Исправление
Scheduler ядра не изменён. Команда `exec SONARVWR.EXE` сохраняет привычную синхронную семантику, но shell запускает только этот read-only observer через существующий `SYS_PROCESS_SPAWN` как временный изолированный MT-процесс и ждёт его PID через `SYS_PROCESS_WAIT`. Поэтому `exe_active` не устанавливается, SONARLOG/SONARPUB продолжают получать MT-кванты, а viewer имеет собственные CR3/image/stack и FAT handle.

## Регрессия
`TESTVWR.TST` усилен: сначала SONARVWR запускается асинхронно через `spawn`, затем проверяется рост heartbeat SONARLOG ещё до ожидания завершения viewer. После этого многократно проверяется обычная команда `exec SONARVWR.EXE` под активной RT-нагрузкой. Проверки состояния RT/MT, handles и размера `/SONAR.LOG` сохранены.
