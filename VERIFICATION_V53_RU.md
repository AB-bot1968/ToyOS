# VERIFICATION V53 — исправление распознавания COMDRV в exec/execq

Исправлены две ошибки shell v52:

1. `exec COMDRV.EXE 1 SEND TEST.TXT` передавал указатель после неправильного смещения (`line+17` вместо `line+16`).
2. Обычный `execq REPEAT COMDRV.EXE PORT SEND|RECV FILE` попадал в старый filename-only parser. Теперь `exec_queue_command()` распознаёт первый EXE1 `COMDRV.EXE` и передаёт исходную строку в COM-расширенный ABI.
3. Для `execq forever COMDRV.EXE ...` исправлено смещение с `line+14` на `line+25`.

Остальная команда `execq` для обычных EXE1 не изменена.
