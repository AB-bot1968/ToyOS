# ToyOS v67 RTD Stage 3 FIX2 — состав архива

## Контрольная база

`ToyOS v67 RTD Stage 3 FIX1`.

## Исправление

`src/kernel.c` — устранён цикл, при котором после исчерпания RT runtime budget
текущая RT-задача могла повторно выбирать саму себя вместо возврата к shell или
переключения на другую RT-задачу.

## Документация

Все исторические корневые `.md` дополнительно собраны в:

`changes/ProjectHistory/`

RTD-документация организована по этапам в:

`changes/RTD/`

Для текущего FIX:

`changes/RTD/Stage3_FIX2/`

## Тесты

`check_rtd_stage3_fix2.sh` — обязательный focused regression test для текущего FIX.

`build.sh` запускает его после предыдущих RTD regression tests.

## Граница runtime-проверки

QEMU runtime не включён в локальную верификацию, так как в среде разработки
отсутствует `qemu-system-i386`.
