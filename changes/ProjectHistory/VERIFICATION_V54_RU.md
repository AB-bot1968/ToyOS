# VERIFICATION V54 — исправление разбора `exec COMDRV.EXE`

Исправлена ошибка v53, из-за которой команда:

```text
exec COMDRV.EXE 1 SEND TEST.TXT
```

получала сообщение `usage`.

## Причина

`shell_run()` правильно передавал `line+16`, то есть указатель сразу после
префикса `exec COMDRV.EXE `. Однако `exec_com_command()` ошибочно считал,
что указатель указывает на имя EXE, и пытался разобрать четыре токена:

```text
NAME PORT DIRECTION FILE
```

В результате строка `1 SEND TEST.TXT` разбиралась как `NAME=1`,
`PORT=SEND`, `DIRECTION=TEST.TXT`, а `FILE` оставался пустым.

## Исправление

`exec_com_command()` теперь получает уже разобранное имя программы
`COMDRV.EXE` из самого кода shell и разбирает только три аргумента:

```text
PORT DIRECTION FILE
```

Затем формируется тот же 48-байтный блок аргументов:

```text
0x00..0x0F  PORT
0x10..0x1F  SEND или RECV
0x20..0x2F  FILE
```

и вызывается существующий `SYS_EXEC_ARGS`.

## Дополнительное исправление

Для `execq forever COMDRV.EXE ...` в v53 также было ошибочно выбрано
смещение `line+25`. Исправлено обратно на `line+14`, поскольку helper
должен получить строку:

```text
COMDRV.EXE PORT SEND|RECV FILE
```

Обычный `execq REPEAT COMDRV.EXE ...` по-прежнему передаёт исходную строку
после `execq` в `exec_queue_com_command()`, где разбирается `REPEAT` и
`COMDRV.EXE` штатным образом.

## Проверки

Проверка `check54.sh` контролирует оба исправленных смещения, формат
разбора аргументов и специальную ветку `execq`.
