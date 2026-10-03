# ToyOS v67.3 — Verification

## Статическая проверка

Проверено наличие ASCII upper-case нормализации в:

- `src/comdrv.c`;
- `src/netdrv.c`;
- `src/loader.c`;
- `src/vgadrv.c`.

## Компиляция

Изменённые исходники компилируются с теми же флагами:

`-Os -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-builtin -fno-tree-vectorize -fno-tree-slp-vectorize -mno-sse -mno-sse2 -mno-mmx -mno-80387 -nostdinc -nostdlib`

В текущей Linux-среде host GCC выдаёт только ожидаемые предупреждения о приведении указателей к 32-битному `uint32_t`; ошибок компиляции нет. Полная проектная сборка по-прежнему требует предусмотренного проектом i686/W64DevKit toolchain.

## Поведенческие сценарии

Ожидаются эквивалентные результаты для верхнего, нижнего и смешанного регистра текстовых параметров. Нормализация выполняется только в драйверах перед сравнением/открытием и не изменяет пользовательскую строку shell.
