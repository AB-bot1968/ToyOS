# ToyOS v67 RTD — Stage 3 FIX2 — план тестирования

## 1. Сборка

Собрать проект штатным `build.sh` под 32-bit W64DevKit/i686.

Контрольные условия:

- `kernel.bin` не превышает 65536 байт;
- FAT16 содержит `RTD.EXE` и `SENSOR.EXE`;
- все обязательные EXE1-проверки проходят.

## 2. Фокусированные статические проверки

    sh check_rtd_stage1.sh
    sh check_rtd_stage1_fix3.sh
    sh check_rtd_stage1_fix6.sh
    sh check_rtd_stage2.sh
    sh check_rtd_stage3.sh
    sh check_rtd_stage3_fix1.sh
    sh check_rtd_stage3_fix2.sh

## 3. Одна RT-задача — обязательная регрессия FIX2

    exec RTD.EXE SENSOR.EXE 20 20 3

Проверить:

- задача стартует;
- появляются `SENSOR: tick`;
- shell не зависает;
- `ESC` приводит к `SENSOR: ESC -> stopped`;
- возвращается `toy0>`;
- после этого shell принимает команды.

## 4. Разные периоды

Повторить для:

    exec RTD.EXE SENSOR.EXE 40 40 3
    exec RTD.EXE SENSOR.EXE 100 100 3

Ожидаемые интервалы соответствуют сетке PIT 20 ms.

## 5. Две RT-задачи одновременно

    exec RTD.EXE SENSOR.EXE 20 20 3

после возврата shell:

    exec RTD.EXE SENSOR.EXE 40 40 7

Проверить, что обе задачи остаются runnable по своим периодическим release,
а завершение одной не переводит вторую в `EXIT`.

## 6. Завершение первой из двух задач

Нажать `ESC` в момент, когда первая RT-задача является текущей.

Проверить:

- текущая задача печатает `SENSOR: ESC -> stopped`;
- вторая RT-задача продолжает выполняться;
- `toy0>` появляется только после завершения всех detached RT-задач.

## 7. Сценарий последовательного запуска без RT

Проверить существующие:

- `exec HELLO.EXE`;
- `execq ...`;
- `execmt ...`;
- обычные shell-команды.

## 8. Runtime ограничения среды разработки

Boot/runtime в QEMU из контейнера не подтверждается, если отсутствует
`qemu-system-i386`. В таком случае runtime-результат должен быть подтверждён
на целевой машине или в локальном QEMU.
