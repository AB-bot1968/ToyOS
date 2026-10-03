# Проверка FIX37A

FIX36E остается стабильной контрольной версией. FIX37 отвергнут.

1. Загрузить FIX37A, выполнить `ls` и убедиться, что `ARGVDIAG.EXE` виден в текущем каталоге. Если cwd не корневой, выполнить `cd /`.
2. Выполнить `spawn ARGVDIAG.EXE ONE TWO THREE`.
3. Норма: `spawn: PID=N`. Если вместо PID появляется `spawn: FAIL <ПРИЧИНА>`, записать ПОЛНУЮ строку: FIX37A теперь не скрывает код загрузчика.
4. После успешного spawn: `ps`, определить MT-слот; `mtdata MTn` должен содержать `ARGV:ARGVDIAG.EXE|ONE|TWO|THREE`; `wait N` должен завершиться NORMAL status 0.
5. Повторить spawn 3 раза с разными аргументами; PID должны отличаться.
6. Проверить 4 MT + spawn, затем 4 RT + 4 MT + spawn; `mtstat`, `rtstat`, `ps`, `wait`, `mtstop ALL`.
7. Проверить `rtstat watch`/ESC и `mtstat watch`/ESC.
8. Регрессия FIX36E: `test TESTCORE.TST`, CRLF, wait активного RT/MT и отмена wait по ESC.

Важно: если первый spawn всё еще не запускается, точная строка `spawn: FAIL ...` теперь локализует слой ошибки без зависания/повреждения MT session. Не считать FIX37A стабильным до полного runtime-теста.
