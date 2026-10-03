# Проверка ToyOS v67.11-B FIX53

## 1. Сборка
Запустить `build.sh` в штатном x86 W64DevKit. Сборка должна вывести рассчитанные `kernel_sectors`, kernel reserve, protected gap, FAT16 LBA, FAT16 volume/image sectors и завершить `layoutcheck`, `FIX53-LAYOUT-VARIANTS` и `FIX53-LAYOUT CHECK` с PASS.

Сборка автоматически проверяет raw kernel end и linker BSS end. Настраиваемый `KERNEL_LOAD_LIMIT` не может быть выше `0xA0000`, то есть kernel не может молча войти в VGA/последующие runtime-области. Если этот диапазон станет недостаточен, это отдельная задача изменения loader/memory map.

## 2. Основной runtime-тест
В QEMU выполнить:

`test TESTLAY.TST`

Тест через `SYS_LAYOUT_INFO=69` проверяет checksum/version LAY1, структурный kernel LBA, выравнивание FAT согласно descriptor, отдельность kernel reserve и protected gap, допустимый memory range, согласованность размеров descriptor и реальную файловую create/write/read/delete операцию после dynamic FAT16 mount. Значения build-policy 25%/64/128/256 не зашиты в TST как ABI-константы.

После теста система должна остаться NORMAL без MT/RT задач, process handles и armed watchdog.

## 3. Полный regression
После TESTLAY выполнить:

`test TESTLOAD.TST`
`test TESTKEY.TST`
`test TESTEVT.TST`
`test TESTLOG.TST`
`test TESTBOOT.TST`
`test TESTSAFE.TST`
`test TESTPOL.TST`
`test TESTHWWD.TST`
`test TESTHLTH.TST`
`test TESTSFP.TST`
`test TESTHB.TST`
`test TESTWD.TST`
`test TESTSUP.TST`
`test TESTRES.TST`
`test TESTPROC.TST`
`test TESTCORE.TST`
`test TESTRCV.TST`
`test TESTACC.TST`
`test TESTRCM.TST`
`test TESTRST.TST`
`test TESTLAY.TST`

`TESTCOM.TST` удалён и больше не входит в regression.

## 4. Автоматические host/build варианты
`build.sh` сам запускает `check_fix53_layout_variants.sh`. Он строит минимум два образа с разным размером kernel/file set и free-space policy, подтверждает увеличение образа и корректный FAT16, затем проверяет обязательный отказ для повреждённого descriptor и kernel memory overflow.

Для дополнительной ручной проверки build-policy можно менять, например, `KERNEL_RESERVE_MIN`, `LAYOUT_GAP_SECTORS`, `FAT_ALIGN_SECTORS`, `FAT_FREE_PERCENT`; runtime TST должен продолжить проверять фактически записанную политику из LAY1, а не значения по умолчанию.

Architecture Vision обновлён. Port-I/O ABI, RT/MT scheduler policy и FIX51 Recovery Manager в FIX53 не изменены.
