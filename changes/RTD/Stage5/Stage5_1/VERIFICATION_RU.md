# ToyOS v67 RTD Stage 5.1 — результаты верификации

## Результат

Stage 5.1 focused test: **PASS**.

## Проверенные компоненты

```text
src/rt_deadline.h          PASS
src/kernel.c               PASS
src/rt_deadline_diag.c     PASS
src/rtd.c                  PASS
src/rt_sensor.c            PASS
src/rt_sensor_diag.c       PASS
```

## Host unit-test

```text
RTD Stage 5.1 absolute deadline checks passed
```

## RTD regression chain

```text
Stage 1                      PASS
Stage 1 FIX3                 PASS
Stage 1 FIX6                 PASS
Stage 2                      PASS
Stage 3                      PASS
Stage 3 FIX1                 PASS
Stage 3 FIX2                 PASS
Stage 3 FIX3                 PASS
Stage 3 FIX4                 PASS
Stage 3 F10                  PASS
Stage 4.1                    PASS
Stage 4.2                    PASS
Stage 4.3                    PASS
Stage 4.4                    PASS
Stage 5.1                    PASS
```

## Build/image verification

Адаптированная локальная сборка W64DevKit target прошла до regression-chain.
Образ содержит:

```text
RTD.EXE
SENSOR.EXE
SENSOR1.EXE
SENSOR2.EXE
DEADLINE.EXE
```

Проверка FAT16 для `DEADLINE.EXE`: PASS.

`kernel.bin` после изменения: 53984 bytes, что ниже лимита 65536 bytes.

## Известное ограничение среды

В локальном Linux-окружении отсутствует `qemu-system-i386`, поэтому фактический boot/runtime
Stage 5.1 в QEMU здесь не выполнялся. Runtime-сценарий приведён в TEST_PLAN_RU.md для
проверки на реальной системе ToyOS.

Полная `build.sh` в этом окружении достигает legacy `check13.sh`, который использует
устаревший текстовый grep для EXE stack mapping; эта архивная проблема существовала до
Stage 5.1 и не относится к RT deadline logic.
