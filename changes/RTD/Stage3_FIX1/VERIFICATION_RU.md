# ToyOS v67 RTD — Stage 3 FIX1 — протокол проверки

## Контрольная база

Изменение выполнено непосредственно от рабочей версии:

`ToyOS v67 RTD Stage 3`

которая ранее была создана от зафиксированной:

`ToyOS v67 RTD Stage 2 FIX2`.

## Шаг 1. Воспроизведение ошибки

Команда пользователя:

    exec RTD.EXE SENSOR.EXE 20 20 3

Наблюдаемый результат Stage 3:

    RTD: start FAIL code=4294967295

`4294967295 = 0xffffffff = -1`, то есть `EXE_ERR_BUSY`.

## Шаг 2. Трассировка возврата -1

В `rt_start_task()` единственный новый для Stage 3 участок, возвращающий
`EXE_ERR_BUSY` перед загрузкой программы, — выбор свободного RT-слота.

До исправления:

    state == TASK_STOPPED || state == TASK_EXIT

После инициализации ядра новые записи `sched_tasks[]` имеют:

    state == TASK_CREATE

Поэтому первый RT-слот ошибочно считался занятым.

## Шаг 3. Минимальное исправление

Критерий свободного слота расширен:

    state == TASK_CREATE ||
    state == TASK_STOPPED ||
    state == TASK_EXIT

Никакие другие условия запуска не менялись.

## Шаг 4. Регрессионная проверка

`check_rtd_stage3.sh`:

    RTD stage 3 checks passed

Дополнительно:

`check_rtd_stage3_fix1.sh`:

    RTD stage 3 FIX1 checks passed

## Шаг 5. Компиляция

32-bit freestanding compilation:

    kernel.c       PASS
    user_shell.c   PASS
    rtd.c          PASS
    rt_sensor.c    PASS

## Шаг 6. Выборочные существующие проверки

    check10.sh     10/10 checks passed
    check3.sh      PASS: Ring-3 frame/GDT/TSS/paging/syscall static checks
                   PASS: VGA hardware cursor and Set-1 keyboard map

## Шаг 7. Полная сборка образа

Адаптированная локальная сборка дошла до создания FAT16-образа:

    kernel.bin = 50285 bytes
    disk.img   = 4194304 bytes
    RTD.EXE    = 1294 bytes
    SENSOR.EXE = 746 bytes

FAT16 проверил наличие `RTD.EXE` и `SENSOR.EXE`.

Встроенный `build.sh` в контейнере остановился на существующих shell-checks из-за
неисполняемых разрешений файлов `check*.sh` в исходном архиве. Это не является
ошибкой компиляции или сборки образа.

## Шаг 8. Runtime

Фактический boot/runtime этого FIX в QEMU здесь не подтверждён: в окружении
нет `qemu-system-i386`.

Основной runtime-тест после сборки:

    exec RTD.EXE SENSOR.EXE 20 20 3

Ожидается `RTD: started ...` вместо `start FAIL code=4294967295`.

## Результат

Исправление локализовано в одном условии выбора свободного RT-слота.
Механизмы `ESC`, `SYS_RT_WAIT`, `toy0>`, PIT 50 Hz и обычный EXE1 loader
не изменялись.
