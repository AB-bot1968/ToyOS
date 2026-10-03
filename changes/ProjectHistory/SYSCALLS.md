# SYSCALLS — Ring-0 shell

## ABI

`INT 80h`; EAX=number, EBX=arg1, ECX=arg2, EDX=arg3, result in EAX.

| # | Name | Arguments |
|---:|---|---|
| 1 | console_write | EBX=buffer, ECX=count |
| 2 | console_read | EBX=buffer, ECX=capacity |
| 3 | timer_get | none |
| 4 | disk_read | EBX=LBA, ECX=buffer, EDX=sectors |
| 5 | disk_write | EBX=LBA, ECX=buffer, EDX=sectors |
| 6 | file_open | EBX=8.3 name, ECX=mode |
| 7 | file_read | EBX=fd, ECX=buffer, EDX=bytes |
| 8 | file_write | EBX=fd, ECX=buffer, EDX=bytes |
| 9 | file_close | EBX=fd |
| 17 | file_size | EBX=8.3 name |

File modes: READ=0x01, WRITE=0x02, CREATE=0x04, TRUNC=0x08, APPEND=0x10.

## Shell demonstration

`syscall 1 Hello` invokes `sys_console_write()` and therefore `INT 80h`. `syscall 3` invokes `sys_timer_get()`. `syscall 2` waits for keyboard input. `cat`, `write` and `append` use the file syscalls rather than calling FAT internals.

## Ring-0 meaning

The shell and dispatcher both execute at CPL0. There is no privilege transition and no user pointer validation. This project is intentionally the trusted-shell stage before the separate ring-3 project.

## Ring-3 boundary

The shell executes with CS=0x1B and user data/stack selector 0x23. INT 80h is permitted because its IDT gate has DPL3. The CPU switches to the TSS kernel stack, the common stub runs the dispatcher, and `iret` restores the user CS:EIP and SS:ESP.

Writable user buffers are restricted to 0x3FF000..0x3FFFFF. Read-only arguments are checked below 1 GiB. Paging is also enabled: kernel pages remain supervisor-only, user text/rodata pages are user-readable and non-writable, and the dedicated user stack page is user-readable/writable.

Команда `filesize FILE` вызывает SYS_FILE_SIZE и возвращает точный размер файла в байтах из записи каталога FAT16. При отсутствии файла или ошибке возвращается 0xffffffff.

Команда `delete FILE` удаляет файл из корневого каталога FAT16, освобождает его цепочку кластеров и помечает запись каталога свободной.

## v31: размер файла

`SYS_FILE_SIZE` принимает указатель на имя файла 8.3 в EBX и возвращает размер в байтах в EAX. Системный вызов не оставляет открытого дескриптора. Имя проходит ту же проверку пользовательской строки, что и `SYS_FILE_OPEN`. Ошибка и отсутствие файла возвращают `0xffffffff`.

Ring-3 оболочка предоставляет команду `filesize FILE`; EXE1 может вызывать INT 80h напрямую или через собственную обёртку.

`QSIZE.EXE` является интеграционным тестом этого syscall.

## v13: запуск EXE

| 11 | exec | EBX=8.3 filename | загружает EXE1 и передаёт управление Ring 3 |
| 12 | exit | — | завершает текущий EXE и возвращает shell |

`SYS_EXEC` читает файл через FAT16, проверяет 16-байтовый заголовок EXE1,
загружает image по `0x00100000`, обнуляет BSS, включает user-доступ к его
страницам и изменяет сохранённый IRET frame. Поэтому возврат из `INT 80h`
попадает сразу в точку входа программы.

`SYS_EXIT` восстанавливает сохранённый frame shell и возвращает управление
на инструкцию после исходного `INT 80h SYS_EXEC`.

## Полный тест из командной строки Ring-3

Команда:

`syscall-test`

выполняет функциональную проверку основных syscall через реальный `INT 80h` из CPL3; интеграционные проверки v23 дополнительно вызывают `SYS_EXEC_QUEUE` и `SYS_QUEUE_STOP`.

Порядок теста:

1. `SYS_CONSOLE_WRITE` — вывод контрольной строки.
2. `SYS_CONSOLE_READ` — ожидается ввод `OK` и Enter.
3. `SYS_TIMER_GET` — два чтения счётчика и проверка монотонности.
4. `SYS_DISK_READ` — чтение одного безопасного временного сектора LBA 129.
5. `SYS_DISK_WRITE` — запись шаблона в LBA 129, чтение обратно, сравнение и обязательное восстановление исходного сектора.
6. `SYS_FILE_OPEN` — создание/очистка временного `SYSCALL.TST`.
7. `SYS_FILE_READ` — чтение записанного содержимого и сравнение.
8. `SYS_FILE_WRITE` — запись контрольной строки.
9. `SYS_FILE_CLOSE` — закрытие дескрипторов после записи и чтения.
10. `SYS_FILE_DELETE` — удаление временного файла.
11. `SYS_EXEC` — запуск `HELLO.EXE`.
12. `SYS_EXIT` — проверяется непосредственно внутри `HELLO.EXE`; дополнительно shell проверяет, что попытка `SYS_EXIT` без активного EXE отклоняется.

LBA 129 выбран потому, что FAT16 начинается с LBA 130, а kernel slot занимает LBA 1..128. Перед записью содержимое LBA 129 сохраняется, после теста сектор восстанавливается и повторно читается для проверки.

Пример:

```text
 toy0> syscall-test

=== SYSCALL TEST ===
[01] SYS_CONSOLE_WRITE: console-write OK
PASS
[02] SYS_CONSOLE_READ: type OK then Enter: PASS
[03] SYS_TIMER_GET: ... PASS
[04] SYS_DISK_READ: PASS
[05] SYS_DISK_WRITE: PASS
[06] SYS_FILE_OPEN: PASS
[08] SYS_FILE_WRITE: PASS
[09] SYS_FILE_CLOSE: PASS
[06] SYS_FILE_OPEN(read): PASS
[07] SYS_FILE_READ: PASS
[09] SYS_FILE_CLOSE(read): PASS
[10] SYS_FILE_DELETE: PASS
[12] SYS_EXIT shell guard: PASS (rejected outside EXE)
[11] SYS_EXEC + [12] SYS_EXIT: Hello from HELLO.EXE (Ring 3)
PASS
=== ALL TESTS PASS ===
```

`SYS_EXEC` и успешный `SYS_EXIT` являются интеграционным тестом: после завершения `HELLO.EXE` управление должно вернуться именно в продолжение shell-команды `syscall-test`.

## v19: проверка консольного ввода/вывода

В v19 отдельно проверяется путь `SYS_CONSOLE_WRITE`, а клавиатура перед входом в
Ring-3 явно инициализируется через 8042. IRQ1 проверяет Output Buffer Full перед
чтением `0x60`; `SYS_CONSOLE_READ` ожидает символ через `sti;hlt;cli`.
Подробности: `CONSOLE_IO_FIX_V19_RU.md`.

## v20: SYS_CONSOLE_READ test correction

`SYS_CONSOLE_READ` is a one-byte/one-call character input syscall. The `syscall-test` now reads `O`, `K`, and Enter using three calls and verifies `OK\n`. This matches the same contract already used by `readline()` for the working `echo` command.

## v22: byte I/O ports

| # | Name | Arguments | Result |
|---:|---|---|---|
| 13 | port_out8 | EBX=port (0..65535), ECX=value (0..255) | EAX=0 or 0xffffffff on invalid arguments |
| 14 | port_in8 | EBX=port (0..65535) | EAX=byte 0..255, or 0xffffffff on invalid port |

These calls are the only intended way for Ring-3 programs to access x86 I/O
ports. The user program does not execute `IN`/`OUT` directly; the CPL0 syscall
dispatcher validates the port/value and performs the privileged instruction.
This keeps IOPL=0 and preserves the Ring-3 privilege boundary.

Shell commands:

```text
outb PORT VALUE
inb PORT
```

Both decimal and `0x` hexadecimal notation are accepted. Examples:

```text
outb 0x80 0x55
inb 0x60
```

Port I/O is hardware-dependent. Reading or writing an arbitrary port can have
side effects on the machine, so the commands are deliberately not included as
a destructive automatic runtime test. `syscall-test` verifies the invalid-range
paths for syscall 13 and 14 without touching hardware.

Standalone EXE programs can use the same INT 80h ABI. `src/hello.c` contains
Ring-3 wrappers `sys_port_out8()` and `sys_port_in8()` demonstrating that an
EXE does not need direct privileged `IN`/`OUT` instructions.

### v22: console syscall result placement

The `syscall` command now starts its result on a fresh console line. In
particular, `syscall 1 TEXT` reports the number of bytes written instead of
mixing the syscall result with the command input line. `syscall 2` keeps its
interactive character prompt, then moves to a new line before printing the
received byte.

## v23: циклическая очередь EXE1

| # | Name | Arguments | Result |
|---:|---|---|---|
| 15 | SYS_EXEC_QUEUE | EBX=packed names, ECX=count 1..25, EDX=repetitions 1..3000 or 0=infinite | EAX=0 on normal completion, 0xffffffff on stop/error |
| 16 | SYS_QUEUE_STOP | none | EAX=0 when stop requested/handled, 0xffffffff when no queue |

`SYS_EXEC_QUEUE` использует фиксированные 13-байтовые слоты имени (12 символов
FAT 8.3 + NUL), поэтому 25 файлов занимают 325 байт. Kernel копирует список в
свою память и последовательно запускает каждый EXE1 через существующий
`SYS_EXEC`/`SYS_EXIT` механизм. После последнего файла индекс возвращается к
нулю; при конечном режиме увеличивается номер полного прохода, при
`EDX=0` очередь работает бесконечно.

Shell commands:

```text
execq REPEAT FILE1 [FILE2 ... FILE25]
execq forever FILE1 [FILE2 ... FILE25]
execq-stop
```

Во время активной бесконечной очереди shell занят синхронным syscall, поэтому
для внешней остановки используется `Esc`. EXE1 может вызвать `SYS_QUEUE_STOP`;
из активного EXE этот syscall немедленно завершает очередь и возвращает shell.


### Вложенный SYS_EXEC_QUEUE

Если `SYS_EXEC_QUEUE` вызывается из EXE1, который уже является элементом
активного `execq`, вызов создаёт дочернюю очередь. Kernel сохраняет родительский
список и позицию в kernel-side stack. После завершения дочерней очереди
родительская очередь восстанавливается и продолжается со следующего элемента.
Поддерживается до 4 уровней вложенности. Если активной родительской очереди нет,
вызов из обычного EXE1 создаёт верхнеуровневую очередь и сохраняет shell frame,
как и раньше.


## v33 timer foundation

`SYS_TIMER_GET` remains syscall 3. IRQ0 at 50 Hz increments the same kernel counter. v33 adds no new syscall and does not modify `execq`; `pit-test` is a Ring-3 shell diagnostic using syscall 3.

## SYS_MT_START = 18

Starts the v34 A/B preemption experiment from Ring 3. It is rejected while an EXE or `execq` is active. The experiment uses one address space and switches contexts only on IRQ0.

## SYS_MT_STOP = 19

Stops the v34 A/B experiment from Ring 3 and restores the saved shell context. It returns `0` on success and `0xffffffff` if the experiment is not active.


## v35 multitasking / CR3

`SYS_MT_START` and `SYS_MT_STOP` retain numbers 18 and 19. v35 does not add another syscall. The ABI of the experiment is unchanged; only the kernel scheduler implementation now switches CR3 between two prebuilt address spaces. Existing syscall numbers and `execq` 15/16 remain unchanged.

## v36 note

Новых syscall нет. Syscall 18 запускает A/B, syscall 19 останавливает эксперимент. v36 изменяет тестовую нагрузку: теперь она фактически проверяет изоляцию памяти между разными CR3.

## v37

Новых syscall нет. SYS_MT_START (18) запускает scheduler test, а SYS_MT_STOP
(19) останавливает его и возвращает оболочке сохранённые CR3 и контекст.
Внутри kernel v37 scheduler хранит состояния READY/RUNNING/STOPPED и
выбирает следующую runnable-задачу по round-robin на IRQ0.


## v38 scheduler syscalls
- `20 SYS_SCHED_BLOCK`: RUNNING -> BLOCKED для текущей Ring-3 task.
- `21 SYS_SCHED_WAKE`: EBX = task id 0..3; BLOCKED -> READY. Повторный wake для уже READY (или другого RUNNING) task считается успешно выполненным: это устраняет гонку с IRQ0 между BLOCKED -> READY.
- `22 SYS_SCHED_EXIT`: RUNNING -> EXIT для текущей task.
Syscalls 1–19 и EXE1/execq ABI не изменены.

### 23 — SYS_EXECMT

`EBX` = packed buffer из 13-байтных имён, `ECX` = количество 1..25. Kernel создаёт до 25 независимых EXE1 scheduler-задач. Каждая получает отдельный CR3, image slot и stack page. Возврат `0` означает, что scheduler запущен. Старые syscalls `11/12/15/16/18..22` сохраняются.

### 24 — SYS_LS

`EBX=PATH` (or `EBX=0` для ROOT). Показывает содержимое указанного каталога одной строкой; каталоги отмечаются `/`. Относительный PATH разрешается относительно cwd. Возвращает `0` при успехе и `0xffffffff` при ошибке FAT/ATA. Ring-3 EXE1 programs may invoke the same syscall directly.

## v43 baseline note
`SYS_LS=24` сохраняет номер syscall, но после v60 принимает путь. `EBX=0` оставляет совместимость с ROOT; shell `ls` без аргумента передаёт `.` и поэтому показывает текущий cwd.

## v51: аргументы EXE1 и COMDRV.EXE

### SYS_EXEC_ARGS = 25

`EBX` = имя EXE1, `ECX` = адрес пользовательского блока из 48 байт. Блок
содержит три строки по 16 байт: аргумент 1, аргумент 2, аргумент 3.
Ядро сначала проверяет и копирует эти строки в свою память, затем передаёт
их EXE1 через EBX/ECX/EDX, указывающие на область пользовательского стека.
Старый `SYS_EXEC=11` не изменён.

### SYS_EXEC_QUEUE_ARGS = 26

`EBX` = пользовательский массив записей, `ECX` = число записей 1..25,
`EDX` = число повторений 1..3000 или 0 для бесконечного режима. Каждая запись
имеет 61 байт: имя EXE1 (13 байт) + три аргумента по 16 байт. Очередь остаётся
строго последовательной: после `SYS_EXIT` текущего EXE1 запускается следующий
элемент/повтор.

## COMDRV.EXE1

`COMDRV.EXE` является первым пользовательским драйвером последовательного
порта. Он использует только `SYS_PORT_OUT8=13` и `SYS_PORT_IN8=14` через
`INT 80h`; привилегированные инструкции `IN/OUT` внутри Ring-3 отсутствуют.

Команды shell:

```text
exec COMDRV.EXE PORT SEND FILE
exec COMDRV.EXE PORT RECV FILE
execq REPEAT COMDRV.EXE PORT SEND FILE
execq REPEAT COMDRV.EXE PORT RECV FILE
execq forever COMDRV.EXE PORT SEND FILE
execq forever COMDRV.EXE PORT RECV FILE
```

COM1..COM4 используют стандартные базы `03F8h/02F8h/03E8h/02E8h`.
UART: 115200 бод, 8N1, polling, без UART IRQ. Приём создаёт/очищает выходной
файл и завершается после 1 секунды тишины после первого байта; если поток не
начался, действует предел 5 секунд. Передача имеет тайм-аут ожидания готовности
THR, поэтому отсутствие устройства не превращается в бесконечное зависание.


### v57 — LOADER.EXE и вложенная очередь с аргументами

`SYS_EXEC_QUEUE_ARGS = 26` получил минимальное расширение: Ring-3 EXE1,
который уже выполняется (`exe_active=1`), может создать аргументную очередь.
Если в этот момент уже активна родительская очередь, её состояние сохраняется
тем же механизмом `queue_save_parent()`, который используется старым
`SYS_EXEC_QUEUE`.

Дополнительно `SYS_EXIT` для EXE1 теперь использует `EBX` как код завершения
элемента очереди:

- `EBX=0` — элемент завершился успешно, запускается следующий;
- `EBX!=0` — текущая очередь останавливается, следующий элемент не запускается.

Это изменение позволяет `LOADER.EXE` безопасно построить цепочку:

```text
COMDRV.EXE PORT RECV FILE.EXE
FILE.EXE
```

Если COMDRV завершился ошибкой, `FILE.EXE` не запускается.

`LOADER.EXE` является обычной Ring-3 программой EXE1 и не требует нового
формата исполняемых файлов, новых привилегий или отдельного kernel loader.

## v60 — каталоги и пути

`SYS_LS=24` теперь принимает `EBX=PATH`. Для обратной совместимости `EBX=0`
означает корневой каталог `/`. `ls` без аргумента передаёт `/`, а `ls PATH`
показывает содержимое указанного каталога. Каталоги выводятся с суффиксом `/`.

`SYS_MKDIR=27`: `EBX=PATH`, создаёт каталог с атрибутом FAT16 `0x10` и записями
`.`/`..`. Возвращает `0` при успехе и `0xffffffff` при ошибке.

`SYS_RMDIR=28`: `EBX=PATH`, удаляет только пустой каталог. Непустой каталог,
обычный файл и служебные `.`/`..` удалить нельзя. Возвращает `0` при успехе и
`0xffffffff` при ошибке.

Начиная с v60 `SYS_FILE_OPEN`, `SYS_FILE_DELETE`, `SYS_FILE_SIZE` и `SYS_EXEC`
разрешают пути вида `/BIN/HELLO.EXE`. LFN не поддерживается; каждый компонент
должен соответствовать FAT 8.3.


### 29 — SYS_CHDIR

`EBX=PATH`. Проверяет существование каталога после нормализации пути и устанавливает
текущий рабочий каталог ядра. Возвращает 0 при успехе, `0xffffffff` при ошибке.

### 30 — SYS_GETCWD

`EBX=BUFFER`, `ECX=CAPACITY`. Копирует канонический cwd с завершающим NUL в
пользовательский буфер. Возвращает длину пути без NUL; при недостаточной ёмкости
или недопустимом user buffer возвращает `0xffffffff`.

## v61 path semantics

Все файловые операции используют общий нормализатор: относительный путь разрешается
от cwd, абсолютный путь начинается с `/`, `.` удаляется, `..` поднимает на уровень
выше, а переход выше ROOT остаётся в ROOT. Формат FAT16 и 8.3 не меняются.


### v64
- `31 SYS_FILE_COPY`: `EBX=SOURCE`, `ECX=DEST`, копирует обычный файл.
  Относительные пути разрешаются относительно kernel-side CWD. Каталоги SOURCE запрещены.

## v65 video

- `32 SYS_VIDEO_MAP` — для текущего обычного Ring-3 EXE1 временно открыть
  user-доступ к VGA framebuffer `0xA0000..0xAFFFF`.
- `33 SYS_VIDEO_UNMAP` — закрыть этот user-доступ.
- `34 SYS_EXEC_ARG` — получить аргумент текущего EXE1. `EBX=INDEX` (0..2),
  `ECX=USER_BUFFER`, `EDX=BUFFER_SIZE` (не менее 16). Ядро копирует сохранённую
  строку аргумента в пользовательский буфер и возвращает её длину без NUL.
  Это безопаснее прямой передачи аргументов через входные регистры.

`VGADRV.EXE` использует также существующие `13 SYS_PORT_OUT8` и
`14 SYS_PORT_IN8` для программирования VGA-регистров. IOPL не повышается.


## v65.6: восстановление текстового VGA

`SYS_VIDEO_TEXT = 35` — возвращает VGA в стандартный цветной текстовый режим 80x25 и очищает текстовый буфер. Вызов разрешён только активному обычному Ring-3 EXE1 после `SYS_VIDEO_MAP`; переключение выполняется ядром, а не пользовательской программой.

### RTD Stage 5.3

`SYS_RT_JOB_INFO = 44` — read-only diagnostic information for the current detached RT job: job sequence, absolute release tick, absolute deadline tick, current timer tick, next release tick, active/missed flags, cumulative deadline misses and skipped releases. Existing RTD syscall numbers and `SYS_RT_DEADLINE_INFO = 42` remain unchanged.
