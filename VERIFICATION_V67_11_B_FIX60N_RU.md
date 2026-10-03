# ToyOS v67.11-B FIX60N — EXECMT scheduler / SONAR physical-path correction

## Найденные ошибки
1. FIX60K откладывал `sched_active` до будущего `SYS_CONSOLE_READ`. Поэтому автоматический `RUN execmt ...` мог видеть READY-задачу, которая фактически ещё ни разу не исполнялась. TESTSONK/L/M давали ложноположительное подтверждение.
2. TESTMTK/TESTMTL/TESTSONK/TESTSONL/TESTSONM не включались в FAT16 image; checker-ы FIX60I..M не входили в основной build acceptance chain.
3. Старый FIX55 checker всё ещё требовал включение THRE IRQ, хотя FIX60H перевёл физический syscall-70 transport на polling. Он печатал PASS до последующей failing-проверки, что вводило в заблуждение при чтении вывода.
4. Успешный `SYS_DATA_CHANNEL/op2` всё ещё вызывал `serial_time_us()` и тем самым latch PIT0. Для sensor publish это лишняя связь с scheduler timer.
5. PIC IRQ4 оставался размаскирован, хотя UART IER=0 и физический transport уже полностью polling.
6. SONARSIM читал COM как строго выровненные блоки по 8 байт. Один старый/потерянный байт мог навсегда сдвинуть границы всех следующих Modbus-запросов.

## Исправления
- EXECMT становится runnable атомарно в конце успешной подготовки. Interrupt gate не допускает IRQ0 внутри syscall; первый реальный dispatch выполняется только последующим IRQ0 из Ring3 и захватывает актуальный foreground frame.
- Удалён скрытый console-read commit.
- Data Channel timestamp переведён на монотонный `rt_time_ticks*10000` без PIT latch.
- IRQ4 оставлен masked в PIC для polling transport; обработчик сохранён, syscall 13/14 не изменены.
- SONARSIM теперь использует sliding 8-byte window: неверный кадр сдвигается на один байт до следующего валидного CRC/адреса/function request. Стартовая строка содержит `READY` и немедленно flush-ится.
- Добавлен структурный ASSERT `MT_PROGRESS slot min`, который ждёт timer progress и проверяет реальные dispatch/cpu_ticks.
- Добавлены TESTMTN.TST и TESTSONN.TST; они требуют фактического выполнения MT/SONARDRV и возврата foreground.
- TESTMTK/L и TESTSONK/L/M теперь реально копируются и упаковываются в FAT16.
- checker-ы FIX60E..N включены в build acceptance до финального `BUILD AND VERIFICATION OK`.

## Порядок физического запуска
1. Создать/проверить com0com COM1 <-> COM2.
2. Запустить SONARSIM на COM2 и дождаться строки `SONARSIM: COM2 115200 8N1 slave=1 mode=0`.
3. Запустить QEMU с `-serial COM1`, дождаться `toy0>`.
4. Выполнить `execmt SONARDRV.EXE`.

QEMU/ToyOS может быть запущен до SONARSIM, но SONARDRV следует запускать после готовности SONARSIM. Если peer отсутствует, SONARDRV обязан ограниченно timeout/retry; shell не должен зависать.

## Обязательные новые тесты
```
test TESTMTN.TST
test TESTSONN.TST
test TESTPHY.TST
test TESTUART.TST
test TESTSON.TST
test TESTSIM.TST
```
Все: `FAIL=0`. Затем полный regression suite.

Architecture Vision обновлён: polling SONAR transport не использует IRQ4; Data Channel timestamp для sensor path — coarse monotonic 100-Hz epoch. Политика syscall 13/14 не изменена.
