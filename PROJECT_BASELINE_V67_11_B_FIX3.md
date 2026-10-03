# Project Baseline — ToyOS v67.11-B-FIX3

Базовая линия сохраняет v67.11-A как основу файловой системы: FAT16 начинается
с физического LBA 512. v67.11-B и последующие FIX используют эту схему.

Сохраняются syscall/ABI 1..48, i386 32-bit freestanding архитектура, case-insensitive
команды, фоновые SENSOR1..SENSOR4, `rtstat stop`, `rtstat watch`, `rtstat dump`,
`rtstat compare`, `rtstat reset`.

FIX3 добавляет только исправления консольной тишины SENSOR и shell/RT context handoff.
