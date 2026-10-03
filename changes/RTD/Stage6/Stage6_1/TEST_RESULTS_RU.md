# Stage 6.1 — результаты проверки

## Автоматические focused-тесты

Успешно:

```text
RTD Stage 4.1 priority comparator checks passed
RTD Stage 4.2 priority arbitration checks passed
Stage 4.3 diagnostic source/build assertions passed
RTD Stage 4.3 diagnostic SENSOR1/SENSOR2 checks passed
Stage 4.4 priority-preemption model passed
RTD Stage 4.4 priority-preemption checks passed
RTD Stage 5.1 absolute deadline checks passed
RTD Stage 5.1 FIX1 absolute-deadline immutability checks passed
RTD Stage 5.2 deadline-miss arbitration model passed
RTD Stage 5.2 deadline-miss best-effort checks passed
RTD Stage 5.2 FIX1 exact runtime scenario model passed
RTD Stage 5.2 FIX1 deadline-miss policy checks passed
RTD Stage 5.2 FIX2 recount/status checks passed
RTD Stage 5.3 periodic release/no-backlog checks passed
RTD Stage 6.1 dual-rate timebase model passed
RTD Stage 6.1 dual-rate timebase checks passed
```

## Компиляция

Изменённые RTD-компоненты успешно компилируются как 32-bit freestanding C. В контейнерной среде остаются два известных старых warning `unused-function`, относящиеся к существующему коду `fat_find_free_dir()` и `sys_ls()`; новых warning от Stage 6.1 не добавлено.

## Сборка образа

Адаптированная локальная сборка создала:

```text
kernel.bin = 53984 bytes
kernel slot = 106/128 sectors
RTTIME.EXE = 665 bytes
RTD.EXE = 1367 bytes
SENSOR.EXE = 746 bytes
SENSOR1.EXE = 1085 bytes
SENSOR2.EXE = 1085 bytes
RTDMISS.EXE = 1006 bytes
RTDSKIP.EXE = 947 bytes
```

Все перечисленные файлы прошли `fat16check` и попали в FAT16 root.

После сборки `build.sh` дошёл до существующей `check13.sh`, которая исторически ищет устаревшую точную текстовую форму отображения EXE stack page. Эта остановка не связана с Stage 6.1.

## Runtime

Аппаратный runtime в QEMU в текущем окружении не выполнен: `qemu-system-i386` отсутствует. Поэтому следующие проверки являются целевым runtime-планом, а не заявленным результатом локального boot.
