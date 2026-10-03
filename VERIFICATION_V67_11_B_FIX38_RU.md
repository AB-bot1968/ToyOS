# Проверка ToyOS v67.11-B FIX38

FIX37A является стабильной точкой отката. FIX38 не считать стабильным до runtime-проверки.

## 1. Автоматический тест владения ресурсами
После загрузки:

    resstat
    test TESTRES.TST
    resstat

До и после теста ожидается `PROCESS HANDLES=0`. TESTRES.TST должен завершиться `FAILED=0`. LEAKFD.EXE намеренно не закрывает файл: очистку обязан выполнить kernel при SYS_EXIT.

## 2. Регрессия spawn/argv/wait

    spawn ARGVDIAG.EXE ONE TWO THREE
    ps
    mtdata MTn
    wait PID
    resstat

Ожидается прежний рабочий результат FIX37A и `PROCESS HANDLES=0`.

## 3. MT/RT регрессия
Повторить ранее подтвержденные тесты 4 MT + 4/8 RT, MTSTAT WATCH, RTSTAT WATCH, ESC, mtstop ALL, rtstat stop SLOTn. После остановок выполнить `resstat`: процессных дескрипторов оставаться не должно.

## 4. Файловая регрессия shell
Проверить ls/cat/write/append/crlf/cp/rm/filesize и `test TESTCORE.TST`. Shell использует legacy owner=0 и не должен потерять доступ к своим дескрипторам.

## 5. Fault containment
Повторить FAULTUD/FAULTGP/FAULTPF сценарии FIX35B/FIX37A. После contained fault выполнить `resstat`; дескрипторы faulted PID должны быть освобождены.

Критерий принятия: нет зависаний/регрессий, TESTRES.TST FAILED=0, существующие TESTCORE/TESTPROC проходят, RT/MT статистика продолжает изменяться штатно.
