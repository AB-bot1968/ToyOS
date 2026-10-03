# Проверка ToyOS v67 RTD Stage3 F10 FIX1

## Найденный дефект

После возврата `VGADRV.EXE` в текстовый режим наблюдался:

`EXC 14 EIP=71016 CS=27 ERR=7 CR2=319488`

`CS=27` — Ring-3. `ERR=7` — страница присутствует, выполнялась запись из user mode. `CR2=319488 = 0x4E000` соответствует области kernel `.bss` в собранном образе. Новые изменяемые переменные shell для F10/RTD действительно попадали в обычную `.bss`.

## Исправление

Shell state перенесён в `.userdata`; linker формирует `.udata`; kernel отображает этот диапазон как `P|RW|US`. Обычная kernel `.bss` по-прежнему не получает `US`.

## Статическая верификация

`sh check_vga_text_shell_userdata.sh` — PASS.

`sh check_rtd_stage3_f10.sh` — PASS.

32-bit freestanding compile:

- `src/kernel.c` — PASS;
- `src/user_shell.c` — PASS;
- `src/rtd.c` — PASS;
- `src/rt_sensor.c` — PASS;
- `src/vgadrv.c` — PASS.

Linker-layout probe показывает:

- `.udata` присутствует;
- `___user_data_start` и `___user_data_end` существуют;
- `.udata` находится отдельно от `.bss`;
- диапазон `.udata` page-aligned.

## Runtime

Загрузочный запуск в QEMU в текущем окружении не выполнен: `qemu-system-i386` отсутствует. Поэтому устранение аппаратного #PF подтверждено статически и по layout, а не фактическим boot-run в данном окружении.
