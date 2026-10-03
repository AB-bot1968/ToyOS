# Проверка FIX40 SOFTWARE WATCHDOG / HEARTBEAT

База FIX39 подтверждена runtime. FIX40 считать кандидатом до выполнения этих тестов.

## 1. Автоматические тесты
В чистой загрузке выполнить:

    test TESTHB.TST

Ожидается FAIL=0, supervisor status 0. HBEATOK.EXE должен регулярно публиковать heartbeat и завершиться нормально без рестарта.

Затем:

    test TESTWD.TST

Тест занимает несколько секунд: HANGWD.EXE намеренно зависает. Supervisor должен обнаружить отсутствие heartbeat, остановить PID, перезапустить его не более трех раз, затем завершиться status=3. Ожидается FAIL=0, MT_ACTIVE=0 и PROC_HANDLES=0.

После этого:

    test TESTSUP.TST
    test TESTRES.TST
    test TESTPROC.TST
    test TESTCORE.TST

Все должны завершиться FAIL=0.

## 2. Ручная диагностика

    supstart HBEATOK.EXE
    supstat

Во время выполнения состояние RUNNING; после нормального завершения supervisor также завершается без рестартов.

    supstart HANGWD.EXE
    supstat

Наблюдать TIMEOUT/WDRESTART, а после лимита WDFAILED. Система и shell должны оставаться отзывчивыми.

## 3. Регрессия scheduler/lifecycle
Повторить проверенный набор FIX39: 4 MT + 8 RT, RTSTAT WATCH/ESC, MTSTAT WATCH/ESC, spawn ARGVDIAG.EXE ONE TWO THREE, wait/ESC, mtstop ALL, fault containment и файловые операции. RT MISS/SKIP не должны начать расти из-за FIX40.

## 4. Важно
SYS_PROCESS_HEARTBEAT=60 и SYS_PROCESS_STOP_PID=61 добавлены после существующих 0..59; старые номера не изменены. Scheduler policy не менялась. Это software watchdog; аппаратного watchdog в FIX40 нет.
