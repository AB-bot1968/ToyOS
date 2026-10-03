# Журнал шага — Stage 6.1 FIX2

1. Взята база Stage 6.1 FIX1.
2. В `SYS_CONSOLE_READ` найдено отсутствие best-effort fallback для READY+MISSED.
3. Добавлен `if(rid<0)rid=rt_pick_ready_missed();`.
4. В диагностическом `SENSOR1/SENSOR2` ожидание tick переведено с 50-Hz
   `SYS_TIMER_GET` на 100-Hz `SYS_RT_TIME_GET`.
5. Добавлен focused regression test.
6. `build.sh` подключён к новому тесту.
7. Все изменённые исходники скомпилированы в i386 freestanding режиме.
8. Выполнены RTD-регрессии Stage 4.4, 5.3 и 6.1.
9. Архив после упаковки распакован и повторно прошёл self-check.
