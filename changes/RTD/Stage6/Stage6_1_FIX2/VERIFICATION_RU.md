# Verification — Stage 6.1 FIX2

## Проверенный дефект

Короткопериодическая job `10/10 ms` могла остаться READY+MISSED во время
блокирующего `SYS_CONSOLE_READ`, поскольку shell выбирал только ON-TIME READY.

## Проверенная новая инварианта

`SYS_CONSOLE_READ` использует правило:

    READY on-time -> first choice
    READY missed   -> fallback only if no on-time job exists

При этом IRQ0, прервавший Ring 0, по-прежнему не выполняет context switch.

## Результаты

- Stage 6.1 FIX2 focused checks: PASS
- 32-bit freestanding `kernel.c`: PASS
- 32-bit freestanding `rt_sensor_diag.c` variant 1: PASS
- 32-bit freestanding `rt_sensor_diag.c` variant 2: PASS
- Stage 4.4 regression: PASS
- Stage 5.3 regression: PASS
- Stage 6.1 regression: PASS
- archive self-check: PASS

## Runtime requirement

QEMU runtime в локальной среде не выполнялся, поскольку `qemu-system-i386`
отсутствует. Проверка реального boot/runtime выполняется на целевой системе.
