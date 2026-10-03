# Проверка ToyOS v67.11-B FIX40A

FIX40A построен поверх подтвержденного FIX40 и меняет только инфраструктуру TEST runner.

## 1. Полностью автоматический нагрузочный тест

В чистой системе выполнить:

    test TESTLOAD.TST

По умолчанию он запускает 25 MT и 8 RT, проверяет `ASSERT MT_ACTIVE 25` и `ASSERT RT_ACTIVE 8`, выводит MTSTAT/RTSTAT, останавливает задачи и проверяет нулевые active/resource counts. Итог должен быть `FAIL=0 PASSED`.

Для другого сочетания используются параметры N. Например в копии TST можно заменить `START MT 25`/`ASSERT MT_ACTIVE 25` на `START MT 4`/`ASSERT MT_ACTIVE 4`; RT допускает 1..8, MT 1..25.

## 2. Условно-интерактивный путь KEY

В чистой системе выполнить:

    test TESTKEY.TST

Тест использует TYPE + KEY ENTER для подачи обычных shell-команд, затем KEY F10 для запуска восьми подготовленных RT задач. По умолчанию проверяется 4 MT + 8 RT. Итог должен быть `FAIL=0 PASSED`.

KEY ENTER/F10 не генерируют аппаратный IRQ клавиатуры: они проверяют логические обработчики shell детерминированно. Реальный F10 с клавиатуры после этого желательно проверить отдельно.

## 3. Регрессия стабильного FIX40

    test TESTHB.TST
    test TESTWD.TST
    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST

Все тесты должны дать FAIL=0.

Дополнительно проверить RTSTAT WATCH/ESC, MTSTAT WATCH/ESC и обычный ручной F10.

## 4. Что не изменялось

Нет новых syscall. Не изменялись kernel scheduler, RT release/deadline, MT quantum 20 ms, watchdog/heartbeat, process ownership, FAT16 LBA512 и kernel area 256 sectors.
