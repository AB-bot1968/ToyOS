# Проверка ToyOS v67.11-B FIX39

FIX38 является стабильной точкой отката. FIX39 считать кандидатом до runtime-проверки.

## 1. Автоматический тест
После загрузки выполнить:

    test TESTSUP.TST

Ожидается `FAIL=0 PASSED`. Тест запускает SUPERVIS.EXE, который контролирует FAULTUD.EXE. FAULTUD.EXE должен аварийно завершаться, supervisor должен выполнить не более трех автоматических рестартов и затем завершиться. После завершения не должно оставаться активных MT-задач и process-owned handles.

## 2. Наблюдение вручную

    supstart FAULTUD.EXE
    supstat
    ps

При достаточно быстром вводе supstat может показать RUNNING/RESTART. После исчерпания лимита supervisor завершится; `ps` сохранит EXITED metadata, а MTDATA остаётся диагностическим снимком до конца MT-сессии.

Для нормального завершения:

    supstart EXIT0.EXE

Supervisor должен завершиться без рестартов.

## 3. Регрессия

    test TESTCORE.TST
    test TESTPROC.TST
    test TESTRES.TST

Также повторить ранее проверенные 4 MT + 8 RT, RTSTAT WATCH/ESC, MTSTAT WATCH/ESC, wait/ESC, spawn ARGVDIAG.EXE ONE TWO THREE, mtstop ALL и файловые операции.

## Ограничения FIX39
Supervisor пока контролирует один компонент. Нет watchdog, persistent log, safe/degraded mode и backoff. Это намеренно следующий этап, а не скрытая часть FIX39.
