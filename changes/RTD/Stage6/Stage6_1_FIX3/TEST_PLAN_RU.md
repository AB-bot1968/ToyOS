# TEST PLAN — Stage 6.1 FIX3

## 1. Цель

Проверить, что release/deadline состояние RT-задачи продолжает продвигаться, если IRQ0 пришёл в Ring 0, и что это исправление не возвращает опасный context switch из kernel frame.

## 2. Focused static test

В корне проекта:

`sh ./check_rtd_stage6_1_fix3.sh`

Ожидаемый результат:

`Stage 6.1 FIX3 Ring-0 release progression checks passed`

`RTD Stage 6.1 FIX3 compile checks passed`

Тест проверяет:

- наличие отдельной Ring-0 ветки;
- вызов `rt_release_jobs(now)` до `return`;
- отсутствие `mem_copy/load_cr3` в этой ветке;
- наличие `SYS_RT_WAIT` и периодического `tick` в SENSOR;
- 32-bit freestanding compilation изменённых RT-компонентов.

## 3. Runtime test: 10 ms

1. Загрузить образ с новым `RTD.EXE` и `SENSOR1.EXE`.
2. Выполнить:

`exec RTD.EXE SENSOR1.EXE 10 10 3`

3. Нажать `F10`.
4. Проверять не менее 3–5 секунд.

Ожидается многократная последовательность:

`SENSOR1: tick`

`SENSOR1: tick`

`SENSOR1: tick`

и т.д.

Один `tick` с последующей тишиной означает регрессию FIX3 и тест считается не пройденным.

## 4. Runtime test: ESC

После непрерывного вывода нажать `ESC`.

Ожидается:

`SENSOR1: ESC -> stopped`

`toy0>`

После этого проверить обычную команду shell.

## 5. Runtime regression 20/40/100 ms

Повторить:

`exec RTD.EXE SENSOR1.EXE 20 20 3`

`exec RTD.EXE SENSOR1.EXE 40 40 3`

`exec RTD.EXE SENSOR1.EXE 100 100 3`

Для каждого значения должна продолжаться периодическая печать до ESC.

## 6. Многозадачность

Подготовить:

`exec RTD.EXE SENSOR1.EXE 20 20 3`

`exec RTD.EXE SENSOR2.EXE 40 40 7`

`F10`

Убедиться, что обе задачи продолжают получать CPU.

## 7. Ограничение проверки

Фактический boot/runtime в текущем Linux-контейнере не выполняется, поскольку `qemu-system-i386` отсутствует. Поэтому runtime-часть должна быть подтверждена на реальном ToyOS/QEMU.
