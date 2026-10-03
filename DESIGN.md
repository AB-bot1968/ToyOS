# DESIGN — Toy OS ring-3 shell

The project preserves the original 32-bit BIOS/protected-mode/FAT16/syscall architecture. The original 10-sector kernel limit is exceeded only because the existing FAT16 write support plus the interactive shell require more space. Loader and image layout are changed consistently to 128 sectors.

## Boot

`boot.S` starts in real mode, saves DL, enables A20 through port 0x92, detects usable RAM with E820 and E801 fallback, stores a saturated uint32 RAM total at 0xB8FF0, and uses INT 13h Extensions AH=42 to load exactly 128 sectors from LBA 1 to 0x7E00. Protected mode uses base-0 code/data descriptors covering exactly 1 GiB. The far jump enters selector 0x08.

## Kernel entry

The first raw kernel instruction remains the top-level `.start` assembly:

`jmpl $0x08, $_kernel_main`

There is no CRT, libc or standard header dependency. `.bss` is NOLOAD and `_zero_bss` clears it before global runtime state is used.

## Hardware

- VGA text mode: 80x25 at 0xB8FF0.
- PIC remapped to 32..47.
- PIT approximately 100 Hz.
- Keyboard set-1, basic US map, Shift, 256-byte ring buffer.
- ATA primary-master PIO LBA28, one-sector operations, IRQ14 (ATA remains masked because the driver is polling) registered.

## FAT16

Test image: 512-byte sectors, 1 sector/cluster, two FATs, 32 sectors/FAT, 512 root entries. Volume begins at LBA 130; data begins at LBA 147. Root 8.3 files support open/read/write/close, create, truncate, append, multi-cluster allocation, FAT mirroring and directory updates. No LFN, subdirectories, seek, delete, rename or journaling.

## Syscalls

`INT 80h` enters the common ISR stub, which saves registers/segments, switches to kernel DS/ES, calls `interrupt_dispatch`, restores state and executes `iret`. ABI: EAX number, EBX/ECX/EDX arguments, EAX result.

## Ring-3 shell

`src/user_shell.c` is a separate compilation unit. It is entered at CPL3 by the documented iret transition after the automatic FAT16/syscall tests and deliberately uses the syscall wrappers for console, timer and file operations. Commands:

- `help`
- `clear`
- `echo TEXT`
- `ticks`
- `cat FILE`
- `write FILE TEXT`
- `append FILE TEXT`
- `read FILE`
- `syscall N [argument]`
- `exit`

The shell is the unprivileged demo component. It cannot execute kernel I/O instructions directly; its intended kernel interface is INT 80h.

## Build/link

W64DevKit is PE/COFF. The x86 kit is required. GCC `.rdata` is merged into `.text` with `objcopy` before the PE/COFF link. `liker.ld` produces one contiguous raw load image; `objcopy` extracts `.text` to `kernel.bin`.

## Verification

`check10.sh` checks ten concrete invariants. `check3.sh` adds three independent passes: source audit, binary/link audit and disk/filesystem audit. Runtime boot is only claimed after an actual i386 emulator test.

## Ring-3 extension

The ring-3 project replaces the final kernel call with an `iret` transition to `_shell_run` at CPL3. The kernel installs a six-entry GDT: null, kernel code descriptor 0x08, kernel data descriptor 0x10, user code descriptor 0x18 (CPL3 selector 0x1B), user data descriptor 0x20 (CPL3 selector 0x23), and TSS 0x28. All code/data descriptors use base 0 with exactly 1-GiB effective coverage. The TSS supplies SS0=0x10 and ESP0=0x001F0000. Paging identity-maps the first 4 MiB and marks only user text, user rodata and the dedicated user stack page as user-accessible.

IDT vector 0x80 is a DPL3 interrupt gate. Hardware IRQ gates remain DPL0. An INT 80h from the shell therefore enters CPL0 using the TSS stack and returns with `iret` to the saved CPL3 frame.

The ring-3 dispatcher validates user pointers. Read-only buffers must stay in the flat address range below 1 GiB; writable buffers are restricted to the demonstration user-stack window 0x3FF000..0x3FFFFF (ESP0 user-stack top = 0x3FFFE0). File names are NUL-bounded to 128 bytes and disk sector counts are bounded before multiplying by 512. This is teaching-level protection; paging provides a real supervisor/user page boundary, but there is still no process/address-space management.

## Paging isolation added in this revision

The previous teaching version used only segmentation for Ring-3 validation. That was not sufficient for real memory isolation because the user data segment still covered the same 1-GiB flat address space. This revision therefore enables 4-KiB paging after the kernel has initialized its BSS.

Only the first 4 MiB are identity mapped by `page_table0`. Kernel pages remain supervisor-only. The linker page-aligns the Ring-3 shell code and read-only data; those ranges receive the user bit in the page table. One dedicated user stack page at `0x003FF000..0x003FFFFF` is also marked user/writable. A Ring-3 syscall buffer is accepted only inside these explicitly mapped user regions.


## Исправление EXC 13 / отсутствие ввода в Ring 3

В предыдущей сборке компилятор мог автоматически генерировать инструкции SSE/MMX (`MOVAPS`, `MOVDQA`, `XORPS` и другие) даже при freestanding-сборке. Для маленького учебного ядра это нежелательно: состояние FPU/SSE ещё не является частью механизма переключения контекста. Поэтому Ring-3 проект теперь явно запрещает автоматическую SIMD/FPU генерацию параметрами `-fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387`. Проверка `check3.sh` дополнительно запрещает появление таких инструкций в итоговом PE/COFF. Это устраняет скрытую зависимость пользовательского Shell от FPU/SSE и является обязательной частью проверки.


## Исправление v7: U/S в PDE

Критическая причина предыдущего сбоя Ring-3: PDE[0] был создан без бита U/S. При этом отдельные PTE уже имели U/S=1. На x86 пользовательский доступ разрешается только если U/S=1 установлен и в PDE, и в PTE. Поэтому page_directory[0] теперь создаётся с `PAGE_US`, а PTE kernel остаются supervisor-only. Это позволяет CPL3 выполнять только отмеченные пользовательские страницы, не открывая kernel pages.


## V7: исправление U/S в PDE

Критическая ошибка предыдущей версии: `page_directory[0]` содержал `P|RW`, но не содержал `U/S`. Отдельные PTE пользовательских областей уже имели `U/S=1`, однако x86 проверяет U/S на каждом уровне таблиц страниц. Поэтому CPL3 не мог получить доступ ни к `.utext`, ни к `.urodata`, ни к user stack. В v7 PDE[0] создаётся с `P|RW|US`; PTE kernel страниц по-прежнему имеют `US=0`, а только пользовательские страницы получают `US=1`.

Для следующего page fault диагностический обработчик дополнительно выводит `CR2`.


## v8: исправление ложного EXC 8 и усиление проверки PIC

Новая ошибка `EXC 8 EIP=8 CS=582 ERR=38467` была разобрана по машинным значениям. `582 = 0x246` — типичное значение EFLAGS, а `38467 = 0x9643` — адрес внутри kernel image. Это означает, что vector 8 был вызван как аппаратный IRQ0 без CPU error-code, но обработчик `isr8` интерпретировал стек как настоящий Double Fault. Причина — конфликт старого BIOS-маппинга IRQ0 -> INT 8 с CPU vector 8 (#DF).

В v8 PIC 8259A полностью маскируется до ICW, затем получает явные вектора `0x20` и `0x28`, cascade `0x04/0x02`, режим `0x01/0x01`, и только после этого устанавливается известная маска `0xFC/0xFF`. Remap выполняется до `paging_init()`, а IRQ0/IRQ1 разрешаются отдельно после PIT. `check3.sh` теперь проверяет весь PIC remap sequence.

Это изменение не ослабляет требования Ring-3: GDT/TSS, точные 1-GiB сегменты, CPL3 IRET, paging U/S, пользовательский stack, INT 80h ABI, FAT16 и Windows 7/W64DevKit build model сохранены.

Команда `delete FILE` удаляет файл из корневого каталога FAT16, освобождает его цепочку кластеров и помечает запись каталога свободной.


## v29 nested sequential queues

The existing sequential `execq` ABI remains unchanged. A `SYS_EXEC_QUEUE` call
originating from an EXE1 that is already inside an active queue is treated as a
child queue. The kernel saves the parent queue state in a four-level kernel-side
stack, executes the child sequentially, then restores the parent and continues at
its next member. A standalone EXE1 may still start a top-level queue and return to
the saved shell frame after it finishes.

The kernel slot is now reserved for 128 sectors (64 KiB): LBA 1..128 is the kernel
slot, LBA 129 is the scratch sector, and FAT16 starts at LBA 130. The build gate
calculates the actual raw kernel size and rejects images larger than the 128-sector
slot.


## v31: SYS_FILE_SIZE

SYS_FILE_SIZE (17) читает размер указанного 8.3-файла непосредственно из FAT16 directory entry. Вход EBX — user pointer на NUL-terminated имя, результат EAX — размер в байтах; 0xffffffff означает ошибку/отсутствие файла. Реализация временно открывает read-only handle внутри kernel, считывает cached size и немедленно закрывает handle, поэтому пользовательский fd не создаётся.


## v33: PIT 50 Hz foundation

The PIT is programmed for 50 Hz using divisor 23864 (rounded from 1193182/50), mode 3. IRQ0 remains vector 32 and only increments `timer_ticks`; it does not switch tasks. The shell command `pit-test` waits for three counter increments and reports the observed transition. No preemptive scheduler or context switch exists in v33.

## v34: first IRQ0 context switch

The v34 experiment deliberately stops before address-space isolation. The timer interrupt switches between two fixed Ring-3 contexts in the same page directory. Each context is exactly 19 words: the unchanged 17-word `struct frame` plus the CPL3 ESP and SS that follow it on the CPU's interrupt stack. The current context is saved on IRQ0 and the other is restored into the live interrupt frame; `iret` then resumes the selected task. Separate user stack pages make A/B independently runnable while keeping CR3 unchanged.


## v35: separate CR3 test spaces

v35 keeps the v34 A/B scheduler experiment but gives each task its own CR3. Two page directories and two first-level page tables are built during paging initialization. Both spaces identity-map the first 4 MiB at the physical level, with supervisor-only kernel pages and user-readable code/rodata pages. The important isolation point is the shared virtual address `0x003fd000`: task A maps it to physical `0x003fd000`, task B maps it to physical `0x003fc000`. Thus a stack access uses the same virtual address but reaches different physical memory after a CR3 switch.

The interrupt frame remains exactly the existing `struct frame` followed by user ESP/SS. CR3 is scheduler metadata, not part of that 19-word architectural frame. Each saved task context therefore stores the 19 words plus its fixed CR3 outside the frame. IRQ0 saves the current frame, selects the other task, loads its CR3, and restores its frame. Stopping the experiment restores the original shell CR3 before returning through the saved shell IRET frame.

## v36: фактическая изоляция памяти A/B

В v35 раздельные CR3 и разные физические страницы уже были структурно проверены. v36 добавляет поведенческую проверку: обе задачи имеют одинаковый виртуальный стек `0x003FD000`, но разные физические backing pages. Перед стартом страницы очищаются. A записывает `0xA5A55A5A`; B читает ту же виртуальную позицию в своём адресном пространстве и обязан не увидеть этот маркер, после чего записывает `0x5AA5A55A`. После нескольких IRQ0-переключений A повторно проверяет свой маркер. Это проверяет именно изоляцию данных, а не только значения CR3.

## v37 scheduler

Планировщик хранит массив `sched_tasks[]`. В отличие от v34/v35/v36, IRQ0
больше не знает, что следующая задача обязательно является «противоположной»
A/B: он сохраняет текущую задачу, ставит ей `TASK_READY`, выполняет
`sched_pick_next()` с round-robin обходом и запускает следующий элемент,
который не имеет `TASK_STOPPED`. CR3 является частью kernel-side metadata
задачи, а 19 слов CPU context остаются в точном формате существующего
interrupt frame. Это подготавливает архитектуру к расширению числа задач без
изменения ABI прерывания.



### v39: EXECMT

Scheduler расширен до 25 task slots. `execmt` загружает каждый EXE1 в отдельный физический 1-MiB image slot под отдельным CR3; виртуальный EXE1 адрес остаётся `0x00100000`. Стек имеет общий виртуальный адрес `0x003fd000`, но отдельную физическую страницу. Загрузка выполняется через временное переключение CR3, FAT16 остаётся тем же.

## v38 scheduler lifecycle
v38 использует четыре TCB: `words[19]`, `cr3`, `state`, `id`, `switches`. CREATE является начальным состоянием, затем задачи переводятся в READY. IRQ0 сохраняет RUNNING context и выбирает READY round-robin. BLOCK переводит текущую задачу в BLOCKED и сразу выбирает READY; WAKE разрешает только BLOCKED task; EXIT переводит текущую task в EXIT, после чего она больше не выбирается. При `SYS_MT_STOP` не завершённые задачи получают STOPPED и восстанавливается shell CR3/context. Каждый TCB имеет собственный CR3 и собственную физическую страницу для общего виртуального стека `0x003FD000`.


### v43: SYS_LS
`SYS_LS=24` проходит по FAT16 root directory, пропускает удалённые/LFN/volume-label/directory записи и выводит имена регулярных 8.3 файлов одной строкой через пробел. Имена используют VGA attribute `0x01` (blue). Shell и Ring-3 EXE1 используют один и тот же syscall.

### v43 baseline
v43 is the project baseline after the repeated loss of shell help entries. `help` must always expose both `ls` and `execmt`. `SYS_LS=24` is the kernel-facing root-directory listing service; it is callable from Ring-3 EXE1 as well as from shell. `ls` prints regular FAT16 root files as one space-separated VGA-blue line.

## v60 — FAT16 каталоги

Каталоги не получили отдельный самодельный формат. Обычный FAT16 directory
entry с attribute `0x10` указывает на кластерную цепочку каталога. `ROOT`
остаётся специальным фиксированным каталогом FAT16. В kernel слой
`fat_lookup_path()` разрешает компоненты пути последовательно: ROOT -> каталог
-> следующий каталог -> конечная запись.

`fat_dir_find_free()` ищет свободную запись и при необходимости расширяет
цепочку подкаталога новым кластером. `fat_mkdir()` выделяет кластер и создаёт
`.` и `..`; `fat_rmdir()` сначала убеждается, что пользовательских записей нет,
и только после этого освобождает цепочку.

v60 намеренно ограничен 8.3 и не вводит cwd. Относительный путь пока считается
относительным к ROOT. Это уменьшает количество изменений в Ring-3 shell и
оставляет v61 для `cd`/`pwd`.


## FAT16 directories — v60.2

Обычные каталоги используют стандартную FAT16 directory entry с атрибутом
`0x10` и собственную кластерную цепочку. ROOT DIRECTORY остаётся фиксированной
областью из 512 записей. Построитель образа обязан просматривать все 32 сектора
ROOT, а не только первый сектор. Пути 8.3 разрешены в форме `/DIR/FILE.EXT`.

В системном образе v60.2 непосредственно в ROOT размещаются `BOOT.BIN`,
`HELLO.EXE`, `COMDRV.EXE`, `LOADER.EXE`, `NETDRV.EXE`, `NET.CFG`, все Q*/MT*
тесты и стандартный `README.TXT`. Вложенные тестовые копии сохраняются в
`BIN/` и `DOC/`.


### v61: cwd / cd / pwd

v61 добавляет kernel-side cwd без изменения FAT16 layout. `fat_normalize_path()`
преобразует абсолютные и относительные пути в канонический абсолютный вид и
обрабатывает `.`/`..`. `SYS_CHDIR=29` меняет cwd только после проверки directory entry,
`SYS_GETCWD=30` безопасно возвращает его Ring-3 shell. Благодаря тому, что существующие
`fat_lookup_path()` и `fat_resolve_parent()` используют один нормализатор, относительные
пути автоматически распространяются на файловые операции и загрузчик EXE1.


## v67 RTD Stage 6.1 — dual-rate time base

The PIT is programmed at 100 Hz for the RT scheduler. A separate logical 50 Hz counter preserves the existing `SYS_TIMER_GET` contract. `rt_time_ticks` is the only clock used by detached RT release/deadline/budget calculations. The legacy EXECMT scheduler is still dispatched at the 50 Hz logical cadence. `SYS_RT_TIME_GET=45` exposes the 10 ms RT counter to Ring-3 diagnostics without changing existing syscall numbers.
