# Verification v52

Исправлена маршрутизация команды `exec COMDRV.EXE PORT SEND|RECV FILE`: обработчик теперь получает строку после полного префикса `exec COMDRV.EXE `, поэтому COM-порт `1` корректно передается COMDRV как первый параметр.

Пример: `exec COMDRV.EXE 1 SEND TEST.TXT` -> COMDRV получает `EBX="1"`, `ECX="SEND"`, `EDX="TEST.TXT"`.

Остальная поверхность команд не изменена.
