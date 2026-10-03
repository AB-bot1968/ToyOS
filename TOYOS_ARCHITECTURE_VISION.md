# Архитектурное видение ToyOS

Этот документ является постоянной частью исходного дерева ToyOS и обновляется вместе с архитектурой ОС. Основной язык документа — русский; устоявшиеся технические термины и имена ABI допускается оставлять на английском.

## Назначение
ToyOS — специализированная ОС длительного автономного управления для бортовых вычислительных систем, способных месяцами или годами работать без непосредственного обслуживания: планетарных роботов, автономных подводных аппаратов, удалённых роботизированных станций и других автономных устройств. Долгосрочная цель — надёжная основа, позволяющая интеллектуальному программному агенту получать данные датчиков, принимать решения, управлять исполнительными механизмами, контролировать своё состояние, обнаруживать отказы и восстанавливаться в пределах доступных аппаратных ресурсов.

## Основные принципы
1. Отказ одного программного компонента, периферийного устройства или канала связи не должен автоматически означать отказ всей системы.
2. Потеря внешней связи является штатно возможным состоянием; критическое автономное управление продолжает работу.
3. Ошибка Ring3 локализуется в процессе, когда это аппаратно и архитектурно возможно; ядро не должно останавливаться из-за локализуемой ошибки пользовательского компонента.
4. Интеллектуальный/планирующий уровень не является единственным механизмом безопасности. Независимые supervisor, watchdog и safe/degraded modes сохраняют контроль при его отказе.
5. Критические пути используют ограниченные и предсказуемые ресурсы: фиксированные очереди/буферы, отсутствие неконтролируемого роста памяти, контролируемые переполнения счётчиков.
6. Существенные faults/events должны поддерживать долговременную регистрацию и последующий анализ.

## Модель исполнения
- RT — критические периодические задачи: датчики, контуры управления, исполнительные механизмы.
- MT — вычислительные, сервисные и драйверные задачи с round-robin планированием.
- LOW/FIFO — будущий низкоприоритетный последовательный уровень для журналирования, диагностики и обслуживания.

Приоритет исполнения и критичность — разные характеристики. В дальнейшем process metadata должна получить отдельный класс criticality/safety. Инвариант ожидания: RT не зависит от MT/LOW; MT не зависит от LOW. Обмен между доменами предпочтительно выполняется через bounded kernel-owned snapshots/queues.

## Ring3 и аппаратный I/O
ToyOS сохраняет архитектурную возможность прямого программирования I/O-портов из Ring3. Целевая модель должна выдавать конкретному процессу/драйверу только необходимые диапазоны портов, предпочтительно через x86 TSS I/O Permission Bitmap, без syscall на каждый IN/OUT. Аналогичный принцип владения применяется к MMIO/IRQ. COMDRV и NETDRV могут оставаться DEFERRED до появления зрелой модели process/resource ownership; их отказ не должен быть обязательной зависимостью основного автономного управления.

## Отказоустойчивость и восстановление
Целевая цепочка: process exit/wait -> spawn/argv -> resource ownership -> supervisor/restart policy -> software/hardware watchdog -> persistent fault/event log -> safe/degraded modes -> Ring3 I/O permissions -> полная стабилизация драйверов. Аппаратный watchdog нельзя безусловно обслуживать только из timer IRQ: работа IRQ сама по себе не доказывает исправность управляющей системы.

## Правила развития
Каждый релиз вносит минимально необходимое изменение и не меняет подтверждённую scheduler policy без отдельной причины. Сначала выполняются статические/host regression проверки, затем W64DevKit/QEMU runtime-проверка по документированной инструкции. Только после успешной runtime-проверки кандидат становится stable. Предыдущие stable-версии сохраняются как независимые точки отката. Этот файл является живым архитектурным документом и начиная с FIX60U ведётся на русском языке.

## Автоматизированная проверка и self-test
Воспроизводимая исправленная ошибка ToyOS по возможности получает автоматический regression test. Тестовая инфраструктура опирается на структурированный диагностический ABI, а не на разбор текста консоли. Developer regression tests должны эволюционировать в POST/BIST/self-diagnostics автономного аппарата. Тестовый механизм не меняет scheduler policy и не становится обязательной зависимостью нормального управления устройством.

## Дисциплина стека и автоматические тесты (FIX36D)
Ring3 control software рассматривает ёмкость стека как ограниченный safety-ресурс. Крупные диагностические snapshots и I/O-буферы test runner не размещаются в глубоких вложенных Ring3-вызовах. Новая тестовая инфраструктура проходит stack-аудит, а воспроизводимые ошибки по возможности закрепляются regression tests. Test runner остаётся инфраструктурой разработки/self-diagnostics и не меняет RT/MT scheduler policy.

## Явные окончания строк текстовых файлов (FIX36E)
Для небольших configuration/test files операции окончания строк явные и детерминированные. Shell-команда `crlf FILE` дописывает ровно CR/LF в существующий файл и не создаёт отсутствующий файл. Это исключает неявный разбор escape-последовательностей в `write` и сохраняет простой способ построения многострочных диагностических файлов на устройстве.

## FIX37 — создание процессов
Detached process creation является явным и PID-based. Первая реализация отображает spawned general-purpose процессы на существующий изолированный MT-класс, сохраняя RT precedence и фиксированный MT quantum. Argument vectors копируются ядром в private address space дочернего процесса; после spawn дочерний процесс не зависит от указателей вызывающего процесса.

## FIX38 — владение ресурсами процесса
Для длительной автономной работы ресурсы имеют явного владельца жизненного цикла. FAT16 handles, открытые Ring3-процессом, принадлежат PID и освобождаются ядром при exit, локализованном fault или явной остановке задачи. Cleanup является lifecycle-инвариантом и не зависит от scheduling policy. Regression tests намеренно создают утечки и проверяют освобождение.

## FIX39 — основа supervisor
Первый supervisor — обычный изолированный Ring3 MT-процесс, а не новая scheduler policy. Он использует spawn/PID и неблокирующий запрос результата. Автоматический restart ограничен тремя попытками; бесконечный restart loop запрещён. Следующие этапы добавляют heartbeat/watchdog, persistent fault log и safe/degraded policy, сохраняя независимость safety-механизмов от AI/mission layer.

## FIX40 — принцип software watchdog
Живой PID не является достаточным доказательством полезного прогресса. Supervised long-running компоненты публикуют явный heartbeat. Пропуск heartbeat обрабатывается как bounded lifecycle failure: owned process останавливается, его ресурсы освобождаются, причина сохраняется и применяется конечная restart policy. Software watchdog отделён от будущего независимого hardware watchdog; активность timer IRQ не является достаточным основанием для его feed.

## FIX40A — постоянная mixed-load регрессия
Детерминированная TEST-инфраструктура поддерживает параметризованный запуск MT/RT load и логические ENTER/F10 shell-events. Это только диагностика: assertions наблюдательные и test runner не меняет scheduler policy.

## FIX41 — bounded журнал lifecycle events
Критическая lifecycle/fault информация сначала записывается в bounded kernel RAM event ring. Fault и scheduler-critical пути не зависят от FAT16 I/O. Persistent storage позже может копировать этот структурированный журнал вне critical path.

## FIX41B — чистота regression
Успешные self-tests освобождают retained execution sessions, живые задачи и ресурсы, чтобы PASS оставлял систему готовой к последующей mission/manual работе.

## FIX42 — persistent diagnostic snapshot
FIX42 добавляет явный FAT16 snapshot bounded RAM event ring. Запись в filesystem остаётся вне interrupt/fault/RT critical paths. Очистка RAM и persistent copy разделены. Binary format имеет version и checksum и служит основой boot/reset diagnostics.

## FIX43 — основа boot/reset diagnostics
FIX43 добавляет versioned/checksummed FAT16 marker намерения перезагрузки `BOOTSTAT.DAT` и сообщает о корректном marker при старте Ring3 shell. Reason codes явные; немаркированная потеря питания не угадывается. Это диагностическая основа, а не hardware reset mechanism, и она не добавляет filesystem I/O в IRQ/fault/RT critical paths. `TESTBOOT.TST` постоянно проверяет persistence и cleanup marker.

## FIX45 — этап supervisor policy
Bounded Ring3 supervisor детерминированно повышает operational state: restartable worker fault/watchdog переводит систему в DEGRADED, исчерпание restart budget — в SAFE. После здорового завершения NORMAL автоматически не восстанавливается. SAFE пока является state/diagnostic foundation и не меняет RT/MT scheduling policy.

## FIX46 — основа hardware watchdog
FIX46 определяет явный control-plane ABI `SYS_HW_WATCHDOG=64` с status/arm/feed/disarm. Текущий backend эмулируется для QEMU/regression и не заявляет независимую физическую reset capability. PIT/IRQ activity не кормит watchdog; feed требует явного действия caller. Будущий board-specific backend может заменить физическую реализацию без изменения syscall numbers или scheduler policy. `TESTHWWD.TST` проверяет lifecycle и чистый disarmed exit.

## FIX47 — основа system health monitor
FIX47 добавляет read-only consolidated snapshot `SYS_SYSTEM_HEALTH=65` поверх уже authoritative kernel state: NORMAL/DEGRADED/SAFE, flags, safe-mode reason, active RT/MT counts, process-owned handles и hardware-watchdog state. Monitor не выполняет recovery и не кормит watchdog; policy отделена от observation. `TESTHLTH.TST` проверяет детерминированную агрегацию и cleanup.

## FIX48 — SAFE admission policy
SAFE получает узкое первое enforcement rule: новые detached SPAWN, EXECMT и RT task admission запрещены. Существующие задачи не уничтожаются неявно, foreground shell diagnostics остаются доступны, DEGRADED ограничений admission не вводит. `SYS_SAFE_POLICY=66` публикует policy flags и denial counters; `TESTSFP.TST` проверяет denial и возврат в NORMAL.

## FIX48A — исправление regression test runner
Ожидаемый SAFE admission denial является корректным policy result, а не ошибкой test directive. TESTSFP проверяет RT denial через обычный staged RTD/F10 path и structured denial counter. Scheduler, SAFE policy и syscall ABI не меняются.

## FIX48B — семантика ожидаемого denial
`EXPECT SAFE_DENY_RT` является успешным TST condition: выполняет normal RTD/F10 path, проверяет ровно один kernel RT denial и отсутствие admitted RT tasks, очищая только намеренно retained pending request. Production SAFE policy и scheduler остаются неизменными.

## FIX51 — managed process recovery lifecycle
Recovery считается успешным не при выборе RESTART или spawn нового процесса, а после containment/termination старого instance, reclaim PID-owned resources, появления нового PID и наблюдаемого heartbeat от него. Restart attempts ограничены FIX50; повторный verification failure переводит в SAFE. Recovery Manager не выполняет FAT16 I/O и не манипулирует scheduler; syscall 68 operations 0..2 остаются ABI-compatible. Port-I/O и RT/MT policy не меняются.

## FIX53 — динамическая разметка диска
FIX53 устраняет фиксированный контракт kernel-slot/FAT-LBA. LBA0 остаётся BIOS boot sector, LBA1 — единственный fixed layout sector. Первые 64 байта содержат versioned `LAY1` descriptor с two's-complement DWORD checksum, остальное — compact real-mode layout loader. Descriptor является единственным runtime source для kernel location/size, reserved growth area, protected gap, FAT16 location/size, image size, alignment policy и safe low-memory loading limit.

Build вычисляет `kernel_sectors` по фактическому `kernel.bin`, резервирует настраиваемый growth space (по умолчанию 25%, минимум 64 сектора), отдельный protected gap (по умолчанию минимум 128 секторов) и выравнивает FAT16 (по умолчанию 256 секторов). Loader читает ровно объявленное число kernel sectors bounded BIOS DAP transfers и отвергает layout, превышающий low-memory limit; build отдельно проверяет linker BSS end. FAT16 geometry вычисляется из набора файлов и requested free-space percentage. Kernel и host verification обнаруживают FAT16 только через LAY1; активные алгоритмы не содержат fixed FAT LBA. Port-I/O, RT/MT и FIX51 recovery policy не меняются.

## FIX54 — укрепление инфраструктуры
Перед реальным supervised sensor driver укреплены legacy SAFE admission, recovery lifecycle validation, administrative process-stop accounting и TST result semantics. Это реализация существующих принципов, а не изменение миссии. Ring3 port I/O через syscalls 13/14 остаётся намеренно unrestricted hardware-access mechanism.

## FIX55 — bounded serial transport
Для последовательных датчиков допускается минимальный Ring0 IRQ transport: короткий IRQ handler переносит байты/ошибки между UART и fixed bounded ring buffers. Протоколы, включая Modbus RTU, интерпретация данных и файловый I/O остаются в Ring3. Transport включается явно; legacy SYS_PORT_IN8/SYS_PORT_OUT8 и свободный Ring3-доступ к I/O ports через эти syscalls сохраняются.

### FIX55A — изоляция UART test/hardware
Детерминированные UART test hooks не включают и не потребляют physical UART interrupts, пока buffered transport выключен. Hardware THRE IRQ arm выполняется только для явно enabled transport. Это сохраняет legacy polling COMDRV ownership model и делает automatic ring-buffer acceptance независимым от external COM timing.

## FIX56 — bounded Sensor/Data Channel
FIX56 добавляет kernel-resident bounded asynchronous data channel для supervised Ring3 drivers и независимых consumers. Channel identity стабилен при restart producer; restart начинает новое generation. Sequence number и timestamp назначает kernel. Каждый reader имеет независимый cursor; отставание глубже ring depth явно возвращается как OVERRUN. Storage фиксирован и FAT I/O отсутствует. Миссия ToyOS и unrestricted Ring3 I/O syscalls 13/14 не меняются.

## FIX57 — Ring3 Modbus RTU protocol layer
Modbus RTU относится к policy/protocol драйвера и остаётся в Ring3. Kernel предоставляет только bounded UART transport и generic Data Channel. Общий Ring3 CRC/frame/timeout-retry module не переносит parser в IRQ/Ring0. Syscalls 13/14 и Ring3 I/O policy не меняются.

## FIX58 — supervised Ring3 sensor drivers
Physical sensor protocol drivers остаются Ring3 services. SONARDRV объединяет bounded Ring0 UART transport, Ring3 Modbus RTU engine и kernel Sensor/Data Channel. Recovery драйвера считается доказанным только после valid Modbus response с CRC, semantic decode, Data Channel publication и heartbeat. Sonar coordinates — signed 32-bit millimetres; каждое значение Modbus занимает два 16-bit register, high word first, без FPU/SSE. UART retries/waits bounded; communication loss не создаёт unbounded resources и FAT I/O в transport/protocol path.

## FIX59 — внешний детерминированный sensor simulator
Windows-программа SONARSIM является host-side deterministic Modbus RTU device model для physical/QEMU acceptance и не входит в trusted kernel. Она генерирует воспроизводимые XYZ/status ответы и позволяет проверять реальный serial boundary отдельно от internal injection. Её отказ не является kernel dependency.

## FIX60 — bounded cyclic telemetry persistence
`SYS_FILE_PREAD/PWRITE` обеспечивают positional FAT16 I/O без изменения legacy sequential handle position; PWRITE ограничен существующим размером и не расширяет файл. `/SONAR.LOG` один раз preallocate до фиксированного размера и используется как bounded ring: два redundant 32-byte headers и 128 records по 32 bytes с sequence, timestamp, signed XYZ mm, status, producer generation и CRC32. Commit order: record, затем alternate metadata header. Generation+CRC позволяют выбрать newest valid header и пережить corruption одного header; transactional FAT16 power-loss atomicity сверх этого протокола не заявляется. Logger — независимый consumer и не блокирует SONARDRV/Data Channel producer. Syscalls 13/14 не меняются.

## FIX60A — root fallback legacy 8.3 executable ABI
Filesystem с cwd/subdirectories остаётся path-aware, но legacy EXEC/EXECMT/SPAWN/RT records сохраняют 13-byte FAT 8.3 executable-name ABI. Простое имя ищется сначала относительно cwd, затем в ROOT; имя с separator fallback не использует. Ordinary file I/O сохраняет обычную cwd semantics. Syscalls 13/14 не меняются.

## FIX60B — проверенный финальный build artifact
`build/toy_os.img` является release artifact, а не intermediate file. `build/disk.img` остаётся working image во время host/static acceptance, а финальный image публикуется только через artifact gate. Успех требует non-empty, byte-identical verified working image, успешный LAY1 check и наличие shipped SONAR driver и executable-loader regression test.

## FIX60D — lifecycle build artifact
Pipeline различает assembled diagnostic boot image и release, принятый regression. После assembly и descriptor/layout checks byte-identical `build/toy_os.img` публикуется атомарно, чтобы поздняя static-check ошибка не уничтожала уже проверенный diagnostic artifact. Runtime architecture, syscall 13/14 и I/O-port policy не меняются.

## FIX60E — изоляция UART IRQ от PIT timebase
UART IRQ handlers не latch/read/reprogram PIT channel 0. IRQ4 ограничен bounded UART status/byte movement и IRQ-safe accounting по уже поддерживаемому coarse RT tick epoch. High-resolution PIT time остаётся доступным вне UART-IRQ syscall context. Modbus retry начинается с чистой RX transaction boundary.

## FIX60F — bounded physical UART IRQ service
Один COM1 IRQ4 entry обслуживает одну reported 16550 interrupt reason и только небольшой bounded byte burst; он не циклически опрашивает меняющийся IIR до quiescence. Остаток FIFO оставляется для reasserted IRQ после EOI, сохраняя bounded latency timer/scheduler/keyboard. Regression TST self-contained: load test сначала устанавливает NORMAL/zero-task/zero-handle/disarmed-watchdog baseline.

## FIX60G — fault containment physical UART RX
Для COM1 Sensor transport physical RX обслуживается bounded polling из SYS_UART_TRANSPORT/read, а не RX IRQ4. Это изолирует подтверждённый QEMU/physical-serial hard-hang path: protocol остаётся Ring3, kernel bounded переносит байты в fixed ring. TX остаётся IRQ-driven. Syscalls 13/14 не меняются.

## FIX60H — полностью polling physical UART transport
Physical COM1 sensor traffic через syscall 70 больше не зависит от IRQ4. RX и TX обслуживаются bounded polling в Ring0 syscall context с fixed rings; UART IER равен zero. Это удаляет host-emulator IRQ reassertion из sensor path, сохраняя syscall 13/14 policy и fixed-memory/no-FAT/no-console discipline. SONARDRV timeout polling использует monotonic 100-Hz epoch.

## FIX60N — deterministic EXECMT activation и polling SONAR isolation
EXECMT preparation/activation были объединены относительно IRQ0: loader завершает task loading до runnable state. MT dispatch остаётся timer-driven из настоящего Ring3 interrupt frame; console-read не выполняет hidden context swap. Acceptance обязана доказывать реальный dispatch/CPU progress. Physical syscall-70 SONAR transport полностью polling: IER=0, PIC IRQ4 masked; vector 36 сохраняется только для historical/ABI compatibility. Data Channel timestamps используют monotonic 100-Hz RT epoch и не latch PIT0. Syscalls 13/14 остаются намеренным решением.

## FIX60O — явный EXECMT commit и bounded host serial boundary
Physical acceptance показала проблему immediate activation FIX60N: timer IRQ мог запустить detached task до печати success line/prompt. EXECMT снова разделён на две фазы: `SYS_EXECMT` загружает PREPARED session (`mt_session_active=1`, `sched_active=0`), отдельный state-only commit делает READY tasks runnable без context switch. Interactive shell commit выполняется из первого `SYS_CONSOLE_READ` после вывода `execmt: started...` и `toy0>`. IRQ0 остаётся normal MT dispatcher; syscall 75 предоставляет тот же commit для deterministic tests.

Sensor acceptance покрывает полную production chain: `Modbus response -> parse -> semantic decode -> Data Channel publish -> process heartbeat -> next cycle`. Test build того же SONARDRV может использовать bounded UART injection hooks, production SONARDRV injected path не содержит. SONARSIM использует bounded overlapped Windows COM I/O с explicit waits/cancellation; `tx=N` означает completed write, `tx=TIMEOUT` — отдельное состояние. Preferred order: SONARSIM READY, затем QEMU/ToyOS, затем SONARDRV; неверный порядок не создаёт unbounded wait.

## FIX60P — UART RX isolation и endurance acceptance
Physical serial diagnostics отделяет ToyOS protocol path от host/emulator serial boundary. Production SONAR transport полностью polling с IER=0/PIC IRQ4 masked. Bounded non-destructive UART diagnostic snapshot показывает только counters/status и не печатает из Ring0/IRQ, не allocates memory, не трогает FAT и не меняет protocol decisions. Deterministic UART hooks работают только при disabled physical transport. SONAR endurance acceptance требует минимум 1000 injected valid response -> parse -> decode -> Data Channel publish -> heartbeat cycles при normal IRQ0 preemption. `UARTRX.EXE` — physical isolation probe: отправляет 8-byte F04 request и считает complete 17-byte response через syscall 70, но не выполняет Modbus parse, XYZ decode, Data Channel publication, FAT I/O или console output.

## FIX60Q — host serial boundary и deterministic physical-probe testing
Automatic `.TST` acceptance не зависит от host serial device, com0com или QEMU host chardev. `UARTRX.EXE` остаётся manual physical boundary probe; `UARTRXT.EXE` собирается из того же source с disabled physical transport и использует bounded UART test hooks. Guest-visible production interface остаётся emulated 16550 COM1 и `SYS_UART_TRANSPORT=70`; host transport не входит в kernel ABI. Для physical SONAR acceptance допускается bidirectional TCP socket chardev вместо проблемного Windows native serial/COM chardev; меняется только QEMU<->SONARSIM host boundary.

## FIX60R — autonomous lifecycle SONARLOG и owned Data Channel readers
FIX60Q остаётся sensor transport baseline. `SONARLOG.EXE` — независимый long-lived consumer и не требует существования producer generation в момент первого CPU; при отсутствии generation или invalid reader он остаётся жив и выполняет bounded Ring3 retry. Heartbeat публикуется после valid reader и после каждого record/header commit. System telemetry file всегда `/SONAR.LOG`, независимо от cwd. Отсутствующий, неверного размера или без valid redundant header telemetry file reinitialize как empty 4160-byte cyclic log. Data Channel reader handles принадлежат PID и освобождаются при forced stop, normal exit, fault containment и watchdog stop вместе с FAT handles.

## FIX60S — deterministic SONARLOG acceptance
SONARTST больше не является primary producer для lifecycle test logger. `SONARPUB.EXE` — paced deterministic Data Channel 0 producer без UART, Modbus, FAT и console path. TESTLGR сначала проверяет real SONARLOG против SONARPUB, full 128-record cyclic wrap с redundant headers/newest record CRC и stop/restart больше DATA_READER_MAX раз; consumer-first startup проверяется отдельно. Только затем unchanged SONARTST+SONARLOG выполняют integration regression. Production separation сохраняется: SONARDRV владеет acquisition/protocol и публикует samples; SONARLOG — independent FAT consumer.

## FIX60T — read-only просмотр SONAR.LOG
`SONARVWR.EXE` предоставляет read-only просмотр `/SONAR.LOG`: выбирает newest valid redundant header, проверяет CRC records и показывает записи в логическом хронологическом порядке. Viewer не создаёт, не исправляет, не truncates и не пишет telemetry file. Все имена файлов проекта подчиняются FAT 8.3; единственное согласованное исключение — `AUTOSTART.SH`.

## FIX60U — единая плоская тестовая инфраструктура
Все runtime `.TST` после сборки находятся только в плоском каталоге `/TST`; вложенные каталоги внутри `/TST` запрещены автоматическим gate. `/TST/ACCEPT.TXT` является manifest полного обязательного набора текущего release и перечисляет каждый shipped `.TST` ровно один раз. Команда `test` без параметров последовательно выполняет manifest; `test NAME.TST` запускает один тест из того же плоского каталога. Каждый новый вызов `test` заново создаёт `/TST/TST.LOG`; pass/fail statistics и итог acceptance записываются туда, а console сохраняет только необходимый progress/diagnostic output для обнаружения зависания. `TST.LOG` не поставляется в image заранее.

Helper-программы, которым для regression требуется тестовый файл, используют абсолютный путь `/TST/...`; обращения к legacy root `.TST` запрещены source gate. FIX60U не меняет syscall ABI, FAT16 format, scheduler policy, SONAR/EXECMT/UART mechanisms или normal application file layout. Начиная с FIX60U фраза «требования к исходному коду те же» включает обязательное создание/обновление автоматических `.TST`, `ACCEPT.TXT`, release gates и русскоязычного архитектурного документа, когда изменение затрагивает архитектуру.

### FIX60U — исправление ёмкости каталога `/TST`
Host-side `mkfat16` обязан создавать обычные FAT16-каталоги как цепочки кластеров, а не как один 512-байтовый сектор. Это необходимо для плоского `/TST`, содержащего полный acceptance-набор: первый кластер каталога содержит `.` и `..`, а дополнительные записи продолжаются по FAT-chain. Runtime FAT16 lookup ToyOS уже обходит цепочку каталогов, поэтому формат диска и kernel ABI не меняются. Build regression обязан не допускать возврата однокластерного ограничения.

### FIX60U: корректная runtime-запись в многокластерный каталог
По результату QEMU-приёмки исправлен общий FAT16 helper `fat_dir_find_free()`: существующая цепочка кластеров каталога полностью просматривается до её расширения. Это сохраняет стандартную семантику directory end marker `0x00` и гарантирует, что созданные runtime-файлы (в частности `/TST/TST.LOG`) остаются достижимыми обычным lookup.

### FIX60U: надёжность журнала приёмочных тестов
Журнал `/TST/TST.LOG` является обязательной частью результата команды `test`. Тестовый runner не имеет права молча прекращать журналирование после временного дефицита FAT handles. Открытие журнала использует ограниченные повторные попытки; неисправимая ошибка записи делает приёмку FAILED. Regression-набор отдельно проверяет повторное открытие с `APPEND` файла, расположенного в многокластерном каталоге `/TST`.


### FIX60U — правило записи acceptance-журнала
`/TST/TST.LOG` не должен зависеть от одной большой растущей FAT16-записи. Test runner накапливает вывод текущего `.TST` в ограниченном RAM-буфере, но сбрасывает его на диск подтверждаемыми порциями по 128 байт. После каждой порции размер файла перечитывается через `SYS_FILE_SIZE`; продолжение разрешено только при точном росте на размер порции. Это делает частичный/неоднозначный commit явной ошибкой и исключает слепое повторение уже записанного префикса. Runtime regression обязан пересекать несколько FAT-кластеров множеством независимых APPEND операций и выполнять полный read-back.

## FIX60ZF — инвариант совместной работы MT, RT и foreground-консоли

Практическое тестирование FIX60ZE выявило важный класс нагрузки: MT-задачи и
RT-задачи по отдельности работают устойчиво, но при их одновременной длительной
работе foreground-консоль могла перестать получать управление, хотя фоновые
SONAR/COM процессы продолжали обмен данными. Это означает не общий deadlock
ядра, а нарушение владения foreground-контекстом.

Архитектурный инвариант ToyOS с FIX60ZF:

1. `SYS_CONSOLE_READ` является границей владения foreground-контекстом shell.
2. Если из ожидающего `SYS_CONSOLE_READ` напрямую запущена готовая RT-задача,
   её `SYS_RT_WAIT` обязан сначала восстановить сохранённый frame shell.
3. RT-задача не имеет права передавать такой ожидающий foreground frame
   непосредственно MT-планировщику. MT получает следующую обычную возможность
   планирования из Ring-3/IRQ0 после возврата shell.
4. Возврат после служебного RT hand-off использует `EAX=2`; shell трактует его
   как внутреннее продолжение ожидания ввода, а не как пользовательский символ.
5. Приоритет RT относительно MT не меняется. Изменяется только правило возврата
   владельцу foreground-контекста, поэтому RT deadline/release policy остаётся
   прежней.
6. Устойчивость MT-only и RT-only недостаточна для приёмки планировщика.
   Обязателен отдельный смешанный тест MT+RT+foreground (`TESTMRT.TST`).

Цель этого правила — исключить starvation интерактивной консоли при длительной
совместной работе фоновых MT-сервисов и периодических RT-задач. Ни MT, ни RT не
должны лишать shell гарантированного возврата к обработке клавиатурной очереди.


## FIX60ZG — bounded interleave для 10-ms RT и постоянно готовых MT

Физическая проверка FIX60ZF выявила более узкий случай: после запуска через F10
задачи RT с периодом 10 ms фоновые `SONARDRV/SONARLOG` могли перестать получать
CPU до следующего события клавиатуры. Причина — `rt_shell_hold` существовал как
намерение дать foreground один interrupt между RT jobs, но применялся только
внутри `SYS_CONSOLE_READ`. IRQ, возникший из Ring-0 `HLT`, не может выполнять
переключение пользовательских контекстов, а следующий настоящий Ring-3 IRQ
снова выбирал READY RT раньше MT.

С FIX60ZG действует следующий инвариант:

1. После `RT_WAIT`, вернувшего управление ожидающему `SYS_CONSOLE_READ`,
   устанавливается `rt_shell_hold`.
2. `SYS_CONSOLE_READ` не снимает этот флаг и не пытается реализовать fairness
   через Ring-0 `HLT`; он возвращает внутренний результат `EAX=2` в Ring-3.
3. Первый следующий PIT IRQ, полученный из настоящего Ring-3 foreground frame,
   атомарно снимает `rt_shell_hold` и ровно один раз запрещает немедленный
   повторный RT dispatch. Release/deadline accounting RT при этом выполняется,
   READY job не теряется.
4. В это одно окно обычный MT scheduler может выполнить готовый MT service.
   После окна обычный приоритет RT над MT полностью восстанавливается.
5. Это правило является bounded fairness, а не снижением RT-приоритета: один
   RT job не может бесконечно замыкать цепочку `console -> RT -> console -> RT`
   и тем самым лишать постоянно готовые MT-сервисы CPU.
6. `TESTMRT.TST` обязан воспроизводить production-сценарий F10 с RT 10/10 ms,
   одновременно держать MT-задачи и подтверждать прогресс обоих классов плюс
   возврат foreground-консоли.

Практический критерий приёмки смешанного scheduler path: отдельно успешные
MT-only и RT-only тесты недостаточны; обязательны одновременно MT progress,
RT completed-job progress и интерактивный foreground при RT 10 ms.

## FIX60ZHD — ограниченный foreground-просмотр телеметрии

Планировщик FIX60ZG принят как проверенная стабильная основа совместной работы RT, MT и консоли. Экспериментальные схемы FIX60ZH–FIX60ZHC, допускавшие дополнительное переключение работающего foreground EXE на MT, в FIX60ZHD не используются: они создавали вложенные continuation и при повторном SONARVWR приводили к повреждению состояния test runner и page fault.

SONARVWR остаётся обычным foreground EXE и читает `/SONAR.LOG` только позиционно (`PREAD`). Один запуск выводит ограниченный снимок последних 8 валидных записей. Это архитектурное ограничение времени владения foreground: диагностический viewer не должен надолго задерживать постоянные MT-службы SONARDRV/SONARLOG. После завершения viewer управление возвращается штатным путём SYS_EXIT, а дальнейшее чередование RT/MT/console выполняет проверенный механизм FIX60ZG.

Инварианты: viewer не изменяет consumer-state SONARLOG; не вводится новый тип асинхронного FG→MT context switch; RT сохраняет установленный приоритет; повторные диагностические запуски должны завершаться через штатный SYS_EXIT без повреждения родительского shell/test контекста.

## FIX60ZE: наблюдатели живых MT-служб

Подтверждённый контракт FIX60ZG сохраняется: обычный foreground EXE не вытесняется MT-задачами. Это исключает вложенные foreground-continuation, которые ранее приводили к повреждению контекста.

Read-only программа, наблюдающая данные постоянно работающей MT-службы, не должна удерживать `exe_active`. Для SONARVWR shell сохраняет интерфейс `exec SONARVWR.EXE`, но реализует его как временный изолированный MT-процесс (`SYS_PROCESS_SPAWN`) с синхронным ожиданием PID (`SYS_PROCESS_WAIT`). SONARLOG при этом продолжает планироваться.

FAT16-дескрипторы принадлежат PID и имеют независимые `start/size/pos`. PREAD/PWRITE являются позиционными операциями одного syscall; переключение MT не происходит внутри FAT-операции. Согласованность циклического SONAR.LOG обеспечивается форматом журнала: запись record выполняется до публикации нового generation в одном из двух заголовков, а reader выбирает валидный более новый заголовок.


## FIX60ZEA: завершение пустой MT-сессии

FIX60ZE выявил ранее скрытую асимметрию жизненного цикла MT-сессии. При административной остановке последней MT-задачи `mt_stop_one()` уже завершал сессию (`mt_session_active=0`), однако при нормальном `SYS_EXIT` последней MT-задачи `sched_exit()` выключал только `sched_active`. В результате в системе не оставалось живых MT-задач, но следующий `EXECMT` получал `BUSY` из-за устаревшего флага сессии.

FIX60ZEA приводит оба пути к одному инварианту: если после завершения MT-процесса не осталось задач в состояниях READY/RUNNING/BLOCKED, MT-сессия закрывается полностью, её счётчик и временные данные очищаются. Если другие MT-задачи остаются живы (например SONARPUB/SONARLOG во время завершения SONARVWR), сессия сохраняется и их планирование не изменяется. RT/MT приоритеты, кванты и правила FIX60ZG не меняются.

## FIX60ZEB: завершение MT-сессии и retained MTDATA

Естественное завершение последней MT-задачи закрывает пустую MT-сессию (`sched_active=0`, `mt_session_active=0`), чтобы следующий `EXECMT` мог стартовать немедленно. При этом диагностический MTDATA завершившейся задачи сохраняется до начала следующей MT-сессии. Это необходимо для постфактум-интерфейсов `WAIT`/`LAST_MTDATA` и соответствует исходному контракту retained snapshot.

Очистка MTDATA выполняется транзакционно перед созданием новой MT-сессии либо явно через `MTSTOP ALL`. Scheduler policy, RT/MT arbitration и кванты FIX60ZG/ZHD не изменяются.

## FIX60ZEC: ограниченная RT-цепочка и post-mortem MTDATA

При смешанной нагрузке RT+MT цепочка прямых передач RT->RT не должна быть бесконечной. Даже если новые 10-мс RT jobs успевают стать READY/MISSED до завершения предыдущей цепочки, один заход из shell/MT ограничивается одним проходом по текущему числу активных RT-задач. После этого восстанавливается сохранённый foreground/MT-контекст. Просроченные RT jobs не теряются и обслуживаются на следующей точке планирования. Это является защитой от starvation, а не снижением RT-приоритета.

Естественное завершение последней MT-задачи закрывает активную MT-сессию, но сохраняет границу завершившейся сессии и MTDATA для post-mortem чтения через WAIT/LAST_MTDATA. Новая MT-сессия очищает эти snapshots транзакционно; `mtstop ALL` очищает их явно.

## FIX60ZED: справедливый RT sweep и живучесть автономного SONARLOG

Длительная конфигурация `2 MT + несколько RT 10/10 мс` выявила ограничение модели FIX60ZEC. Ограничение burst только числом RT-переключений не гарантирует справедливость по слотам: задача высокого приоритета после `SYS_RT_WAIT` могла немедленно получить следующий release в том же tick и снова занять место в burst, пока низкоприоритетный MISSED job оставался без CPU. Дополнительно `rt_wait()` выбирал только обычный READY-класс и не делал fallback на MISSED.

Начиная с FIX60ZED единица справедливости — завершённый RT job конкретного слота. В течение одного burst ведётся mask обслуженных слотов. После завершения job данный слот исключается до возврата управления в shell/MT. Выбор выполняется сначала из обычного READY-класса, затем из MISSED-класса с тем же mask. После возврата в foreground/MT mask очищается и начинается новый sweep. Приоритет продолжает определять порядок внутри множества ещё не обслуженных слотов, но не разрешает одному слоту повторно занять весь burst.

Автономный `SONARLOG.EXE` рассматривается как долговременный сервис. Временный сбой файловой операции не должен превращать сервис в завершившийся процесс. Logger повторно открывает и валидирует `/SONAR.LOG`, при необходимости восстанавливает bounded cyclic-file и продолжает обслуживание Data Channel. Heartbeat сохраняется и в recovery-loop. Явное завершение сервиса остаётся обязанностью управления MT (`mtstop`), а не побочным результатом временного I/O сбоя.

Для интерфейса запуска RT закреплено правило: F10 является commit для pending RT queue. Если пользователь уже полностью набрал `exec RTD.EXE ...`, но вместо Enter сразу нажал F10, эта строка сначала должна быть обработана как RTD prepare и только затем выполняется launch queue. Это устраняет расхождение между видимым числом набранных RT-команд и реально запущенными задачами.

## FIX60ZEE: RT post-sweep slack, явный MT-viewer и справедливый WAIT

FIX60ZEE уточняет модель совместной работы shell, MT и периодических RT-задач с периодом/дедлайном 10 мс.

1. Механизм `rt_shell_hold`, который покупал один MT-квант ценой пропуска следующей RT-диспетчеризации, больше не активируется. Для `period=deadline=10 ms` такой пропуск неизбежно превращался в искусственный deadline miss/skip. После завершения полного RT-sweep MT теперь запускается непосредственно на чистой границе `SYS_RT_WAIT`, используя уже сохранённое продолжение shell. Следующий PIT снова имеет право немедленно запустить RT.
2. `SYS_PROCESS_WAIT` получает явную фазу RT/MT. При непрерывно готовых RT-задачах один RT burst не может бесконечно вытеснять MT-процесс, которого ждёт shell: после RT обязательно резервируется возможность MT-диспетчеризации.
3. Командные классы снова однозначны. `exec` означает foreground EXE1. Live-наблюдатель `SONARVWR.EXE` запускается явно как MT-команда: `execmt SONARVWR.EXE`. Если MT-сессия уже существует, одиночный `execmt` добавляет задачу в неё через существующий изолированный `PROCESS_SPAWN`; многозадачный `execmt` сохраняет прежнюю транзакционную семантику создания новой сессии.
4. Для активно растущего `/SONAR.LOG` промежуточная проверка не должна требовать точного `valid_count`. Введён тестовый инвариант `SONAR_LOG_MIN N`: не менее N согласованно закоммиченных записей плюс валидная последняя запись. Проверка насыщенного кольца `SONAR_LOG_VALID 128` остаётся точной.

Цель этой схемы — не просто отсутствие зависания, а одновременно: отсутствие искусственных 10-мс deadline miss из-за планировщика, гарантированный прогресс MT, предсказуемая консоль и явная семантика запуска viewer.

## FIX60ZEF: строгий владелец foreground и ограниченный MT slack

Результаты длительных 2MT+4RT проверок показали, что одной справедливости RT sweep недостаточно. Интерактивная консоль может потерять продолжение, если фоновые классы запускаются непосредственно из `SYS_CONSOLE_READ` либо если завершение RT автоматически передаёт управление MT после любой foreground-команды.

Начиная с FIX60ZEF вводится правило **один явный владелец foreground continuation**. Обычная команда shell (`rtstat`, `ps`, файловая команда и т.п.) после любого RT preemption обязана продолжиться с точно того же Ring3-frame. Она не является неявной точкой передачи управления MT.

`SYS_CONSOLE_READ` теперь не меняет address space и не запускает RT/MT. При пустой клавиатурной очереди и наличии background work syscall возвращает в Ring3 служебный idle-код. IRQ0 видит настоящий Ring3 frame, после чего RT сохраняет абсолютный приоритет. Если shell действительно находится в idle, после полного RT sweep одна MT-задача может использовать остаток текущего PIT-интервала.

Этот MT slack имеет жёсткую границу: максимум до следующего PIT. На следующем PIT сначала выполняется выпущенный RT sweep, после чего восстанавливается точный сохранённый foreground frame. Если RT job не выпущен, foreground восстанавливается сразу. Таким образом фоновые сервисы используют свободное время между RT releases, но не могут удержать консоль на несколько квантов или заменить её continuation.

Для длинных foreground-операций, которым действительно требуется обслуживание background, используются только явные scheduler boundaries (`SYS_RT_YIELD`, `SYS_PROCESS_WAIT`). Это отделяет политику планирования от случайных точек выполнения команд и делает семантику `exec`/`execmt` и владение console детерминированными.


## FIX60ZEG: сохранение scheduler и контракт фонового вывода

FIX60ZEG фиксирует важный архитектурный инвариант: после подтверждённой стабильности FIX60ZEF изменения UI и тестовой оркестрации не должны менять политику RT/MT scheduler. Поэтому `rt_switch_to_shell()`, `rt_wait()` и `sched_irq_tick()` оставлены байт-в-байт без изменений.

MT-slack считается допустимым только в явно определённых точках: когда интерактивный shell действительно ожидает клавиатуру либо когда foreground явно отдаёт управление через существующий yield-механизм. Автоматические тесты, проверяющие прогресс MT под RT-нагрузкой, обязаны воспроизводить такой idle-shell режим, а не требовать фонового выполнения во время непрерывно выполняющейся foreground-команды `test`.

Для асинхронного консольного вывода введён отдельный UI-контракт, не связанный с планированием. Detached MT/RT slot, выполнивший `SYS_CONSOLE_WRITE`, помечается console-dirty. При завершении slot формирует одноразовый redraw-event. Следующий `SYS_CONSOLE_READ` shell получает существующий код 3, после чего пользовательский `readline()` восстанавливает `toy0> `. Никаких переходов CR3 или подмены frame этот механизм не выполняет.

## FIX60ZEH: тестовый контракт не должен менять стабильный scheduler

После длительных runtime-проверок FIX60ZEG зафиксирован принцип: если RT, MT, SONARLOG, `execmt SONARVWR.EXE` и интерактивная консоль демонстрируют устойчивую совместную работу, исправление неоднозначного acceptance FAIL выполняется сначала на уровне test harness. Изменение scheduler допускается только при прямом runtime-доказательстве ошибки планирования.

`CONSOLE_IDLE_TICKS` моделирует именно idle-shell окно. Начиная с FIX60ZEG интерактивный console-read имеет два нормальных служебных события: код 2 означает реальное idle-окно, код 3 — одноразовый redraw приглашения после завершения background console writer. Redraw не является scheduler failure и не должен разрушать проверку idle. В FIX60ZEH код 3 поглощается test harness, после чего ожидание продолжается до полного заданного интервала; реальные idle-окна по-прежнему подтверждаются только кодом 2.

Тестовый источник является частью воспроизводимости системы. Путь `TST source -> build/TST -> FAT image -> runtime interpreter` должен быть проверяемым. Сборка обязана сравнить staged-копию каждого TST с исходником. При runtime FAIL test runner сообщает размер и FNV-1a32 фактически прочитанного файла, не создавая `TST.LOG` и не сохраняя большой report buffer. Это позволяет отличить реальную ошибку OS от несовпадения test source/образа или ошибочной привязки номера строки.

`TESTMRT.TST` остаётся обязательным тестом смешанного режима. Его назначение — подтвердить совместно 2 MT, 4 RT 10/10 ms, idle-shell background progress, RT fairness и возврат foreground. Удаление теста или снижение его требований не является допустимым способом получить PASS.


## FIX60ZEI: heartbeat-проверка обязана соблюдать idle-shell контракт

FIX60ZEI закрепляет разделение между runtime policy и test orchestration. Подтверждённый scheduler FIX60ZEF/ZEG не изменяется ради acceptance-теста, если runtime-наблюдения показывают устойчивый прогресс RT, MT, SONARLOG и консоли.

`MT_HEARTBEAT_WAIT` исторически являлся foreground busy-poll: он проверял heartbeat и timeout, но сам не создавал idle-shell scheduler boundary. После введения строгого foreground ownership это стало неверным способом тестировать detached MT под непрерывной RT-нагрузкой. Подготовительное `CONSOLE_IDLE_TICKS` не гарантирует, что следующий требуемый heartbeat уже достигнут; если не достигнут, последующий busy-poll фактически прекращает предоставлять MT разрешённый slack.

Для смешанного режима введён отдельный test-only контракт `MT_HEARTBEAT_IDLE_WAIT`: пока требуемый heartbeat не достигнут, test harness воспроизводит штатное ожидание клавиатуры. Console token 2 является реальным idle-shell окном, token 3 — допустимым одноразовым redraw-event и поглощается, остальные результаты считаются ошибкой. Timeout и heartbeat threshold остаются прежними.

Архитектурный инвариант: acceptance-тест не должен требовать от scheduler выполнения background-класса в точке, где production policy намеренно сохраняет foreground ownership. Тест обязан создавать ту же явную scheduler boundary, которая существует в реальном пользовательском сценарии. Исправление тестового контракта не является снижением требований, если сохраняются исходные heartbeat thresholds, timeout, RT fairness и проверки жизненности сервисов.

## FIX60ZEI — frozen Git baseline и политика публикации

После подтверждения 45/45 acceptance-тестов и длительной стабильной совместной работы RT, MT, интерактивной консоли и SONAR FIX60ZEI считается frozen runtime baseline. Подготовка Git-публикации не должна изменять рабочий код, scheduler policy, TST-контракт или runtime resources.

Для публикации вводится отдельный release-integrity слой:

1. Почти всё исходное frozen-дерево FIX60ZEI фиксируется в `FIX60ZEI_FROZEN_CONTENT_SHA256.txt`. Из manifest исключены только файлы, которые намеренно являются частью Git-публикационной обвязки/текущего README и архитектурной документации.
2. `git_preflight.sh` обязан проверять frozen manifest до создания commit или push. Это защищает от случайной правки исходников при подготовке репозитория.
3. Обязательные бинарные ресурсы являются частью исходного проекта, а не build-artifacts. `resources/SPLASH.RAW` и `tools/Python/TEST.BIN` должны явно оставаться tracked несмотря на общие ignore-правила для сгенерированных `.raw/.bin`.
4. `build/`, `dist/`, runtime logs, VM images и credentials не являются частью source history.
5. Первичная публикация создаёт одну ветку `main` и annotated tag `v67.11-B-FIX60ZEI`, указывающий на точный stable release commit. Автоматизация не должна использовать force push или автоматически переписывать уже существующую несовпадающую remote history.
6. Повторный запуск publish automation допускается только идемпотентно для того же commit/tag. Если обнаружена другая локальная или удалённая история, скрипт должен остановиться и потребовать ручного решения.
7. GitHub CI preflight подтверждает целостность source/release tree, но не считается runtime-сертификацией. Полная приёмка ToyOS по-прежнему требует штатной i686/W64DevKit сборки и runtime acceptance в QEMU/целевой тестовой среде.
8. Выбор лицензии не относится к автоматической технической подготовке релиза. Отсутствие `LICENSE` должно быть явно документировано и не маскироваться случайно выбранной лицензией.

Архитектурный принцип публикации: Git infrastructure обслуживает стабильный runtime baseline, а не становится поводом менять его. Любая будущая разработка должна начинаться отдельным новым FIX от tag `v67.11-B-FIX60ZEI`, сохраняя этот commit/tag как неизменную точку возврата.


## FIX60ZEI — авторство и публичный release baseline

Для публичной Git-публикации стабильного FIX60ZEI правообладателем и автором release baseline зафиксирован **Ботнев Александр Валерьевич**. В корне репозитория хранится отдельный `COPYRIGHT` с формулировкой `Copyright © 2026 Ботнев Александр Валерьевич. All rights reserved.`. Это изменение относится только к слою публикации и не меняет runtime-код, scheduler policy, TST-контракт или ресурсы frozen baseline.

Git commit релиза создаётся с локальной repository identity `Ботнев Александр Валерьевич <101033309+AB-bot1968@users.noreply.github.com>`, связанной с GitHub-аккаунтом `AB-bot1968`; глобальная конфигурация Git пользователя не изменяется.

Отсутствие отдельного `LICENSE` трактуется явно: публичная доступность исходников не является автоматической open-source лицензией. Выбор лицензии остаётся отдельным решением правообладателя и не выполняется публикационными скриптами автоматически.

Архитектурный инвариант публикации сохраняется: авторская/репозиторная метаинформация может развиваться отдельно, но frozen runtime baseline FIX60ZEI не переписывается ради оформления GitHub.

## FIX60ZEI: публикация поверх существующего GitHub repository

Для публикации зафиксированного baseline применяется принцип сохранения истории без force-push.
Если удалённый GitHub repository уже содержит bootstrap-коммиты, полное дерево FIX60ZEI
накладывается на текущий `main` одним fast-forward release commit. До и после копирования
обязательно выполняется frozen-preflight; runtime-код и acceptance-тесты не изменяются.
Автор release commit: Ботнев Александр Валерьевич. Release tag: `v67.11-B-FIX60ZEI`.


## FIX60ZEI: Windows-native preflight для публикации GitHub

Публикационная автоматика не должна зависеть от случайного состава POSIX-утилит в Windows shell. Первый запуск `publish_to_existing_github.bat` показал, что наличие `sh.exe` не гарантирует наличие `sha256sum`; требование `sha256sum` в Windows-path ошибочно блокировало публикацию до Git-операций.

Для Windows введён отдельный `git_preflight_windows.ps1`, совместимый с Windows PowerShell 2.0+. Проверка frozen manifest выполняется через .NET SHA-256 и остаётся эквивалентной POSIX-проверке: сверяются все 1109 frozen-файлов, критические hash `src/kernel.c` и `TST/TESTMRT.TST`, acceptance-набор, обязательные бинарные ресурсы, отсутствие build/runtime мусора и типовых credential signatures. POSIX `git_preflight.sh` сохраняется для W64DevKit/Git Bash/Linux.

BAT-wrapper не должен содержать кириллические литералы, от которых зависит Git metadata: старый `cmd.exe` может интерпретировать UTF-8 BAT по OEM code page. Repository-local Git identity задаётся PowerShell-скриптом UTF-8 BOM, поэтому имя `Ботнев Александр Валерьевич` передаётся Git без перекодировки.

Архитектурный инвариант сохраняется: исправления tooling публикации не изменяют frozen runtime baseline FIX60ZEI. Любой preflight failure останавливает процесс до commit/tag/push; force push не используется.


### Побайтовая стабильность Git checkout

Frozen manifest должен оставаться воспроизводимым после обычного `git clone` как на Windows, так и на Linux. Поэтому Git publication-layer запрещает автоматическую конверсию CRLF/LF: `.gitattributes` содержит `* -text`. Сам `.gitattributes` исключён из frozen manifest как изменяемая инфраструктура публикации; manifest защищает 1109 файлов исходного стабильного дерева. Это предотвращает ситуацию, когда Git корректно хранит исходник, но checkout меняет байты BAT-файлов и тем самым создаёт ложное расхождение SHA-256.

## FIX60ZEI GitHub publication: устойчивый shell-auth workflow (2026-10-03)

Для публикации зафиксированного FIX60ZEI принят отдельный безопасный контракт для `publish_to_existing_github.sh`.

1. Аутентификация GitHub проверяется **до** копирования полного frozen-дерева и до создания release commit.
2. Скрипт сначала проверяет URL репозитория через `git ls-remote`, затем выполняет `git push --dry-run origin HEAD:main`. Этот dry-run не изменяет удалённый репозиторий, но подтверждает наличие права записи.
3. Если установлен GitHub CLI (`gh`), скрипт предпочитает его и использует `gh auth status`, `gh auth login` и `gh auth setup-git`.
4. Если `gh` отсутствует, включается интерактивный Git Credential Manager. При первом неуспешном dry-run скрипт дополнительно пытается вызвать `git credential-manager github login` либо совместимый `credential-manager-core` и повторяет dry-run.
5. До подтверждения write-auth никакой release commit/tag не создаётся. Ошибка аутентификации сохраняется в `GITHUB_AUTH_LAST_ERROR.log`, а пользователь получает короткое ASCII-сообщение без зависимости от локальной кодировки терминала.
6. После подтверждения доступа выполняются копирование publication tree, повторный frozen-preflight, один fast-forward release commit, annotated tag `v67.11-B-FIX60ZEI` и обычные `git push` без `--force`.
7. Скрипт идемпотентен: повторный запуск на уже опубликованном FIX60ZEI не должен создавать новый content commit и не должен перемещать существующий tag на другой commit.

Эта логика относится только к инфраструктуре публикации. Runtime-код ToyOS, scheduler, RT/MT policy, TST и frozen baseline FIX60ZEI при данной корректировке не изменяются.

## FIX60ZEI GitHub publication: SSH-only shell workflow (2026-10-03)

После практической проверки на рабочей машине подтверждена успешная SSH-аутентификация GitHub аккаунта `AB-bot1968` с использованием ключа `~/.ssh/id_ed25519`. Для shell-пути публикации FIX60ZEI HTTPS/Git Credential Manager больше не являются частью обязательной цепочки.

`publish_to_existing_github.sh` по умолчанию использует SSH remote `git@github.com:AB-bot1968/ToyOS.git`. Перед любыми release-изменениями скрипт обязан выполнить `git ls-remote` через SSH, а после clone — `git push --dry-run origin HEAD:main`. Только успешное прохождение обеих проверок разрешает копирование frozen tree и создание release commit/tag.

Практическая проверка W64DevKit показала, что его shell не обязан поддерживать MSYS-путь `/c/...`, даже если `C:\Program Files\Git\usr\bin\ssh.exe` реально существует. Поэтому публикационный скрипт больше не проверяет OpenSSH через POSIX `[ -x /c/... ]`. Он сначала использует `ssh` из PATH, а при его отсутствии получает корень установленного Git for Windows через `git --exec-path`, строит native-путь вида `C:/Program Files/Git/usr/bin/ssh.exe` и передаёт его Git через корректно quoted `GIT_SSH_COMMAND`. Для нестандартной установки предусмотрен явный override `TOYOS_SSH_BIN`. Фактической проверкой наличия executable и SSH-аутентификации является `git ls-remote` через SSH.

Отдельно зафиксировано, что BusyBox/W64DevKit `find` может не поддерживать GNU-форму `-size +50M`. Проверка максимального размера публикационных файлов поэтому выполняется переносимым способом через `find` + `wc -c` с порогом 52428800 байт. Это изменение относится только к preflight tooling и не меняет frozen content.

Архитектурный инвариант публикации: authentication transport является внешней инфраструктурой и не должен приводить к изменениям runtime baseline. Неуспешный SSH read/write preflight завершает работу до release commit/tag; force push запрещён. Рабочий код ToyOS, scheduler, RT/MT policy, acceptance-тесты и frozen hashes FIX60ZEI остаются неизменными.
