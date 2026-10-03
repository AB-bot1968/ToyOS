# ToyOS Project Baseline v67.11-B-FIX8

База проекта: v67.11-B-FIX7. Сохраняется FAT16 start LBA 512, i386 freestanding архитектура, фоновые SENSOR1..SENSOR4, SYS_RT_YIELD=49, case-insensitive shell и существующий RT scheduler.

FIX8 изменяет только представление RTSTAT/RTSTAT WATCH/RTSTAT DUMP и исправляет порядок/семантику учёта skipped releases в RT monitor ring. SYS_RT_STATS ABI и формат 8-word ring sample сохраняются.
