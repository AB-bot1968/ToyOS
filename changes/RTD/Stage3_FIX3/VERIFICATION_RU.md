# ToyOS v67 RTD Stage 3 FIX3 — результаты проверки

Дата: 2026-09-18

## Компиляция

- `src/kernel.c` — 32-bit freestanding: PASS
- `src/user_shell.c` — 32-bit freestanding: PASS
- `src/rtd.c` — 32-bit freestanding: PASS
- `src/rt_sensor.c` — 32-bit freestanding: PASS

## Focused regression

- shell-switch guard — PASS
- RT-to-shell sentinel `EAX=2` — PASS
- порядок `sentinel → load_cr3 → frame restore` — PASS
- наличие документации FIX3 — PASS

## Runtime

В текущем контейнере `qemu-system-i386` отсутствует. Поэтому runtime-проверка одновременного ввода/запуска двух RT-задач требует выполнения образа на QEMU или реальном стенде пользователя.
