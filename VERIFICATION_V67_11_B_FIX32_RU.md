# Проверка FIX32

База: стабильная runtime-проверенная FIX31.

## Основные тесты
1. Без RT/MT: `exec VGADRV.EXE SPLASH.RAW`. Картинка должна оставаться до нажатия клавиши, затем `VGADRV: OK` и `toy0>`.
2. Запустить 1 MT, затем ту же команду. Не должно быть `VGADRV: VRAM map failed`; заставка должна ждать клавишу.
3. Запустить несколько/25 MT и повторить тест.
4. Запустить 1 RT и повторить. Заставка не должна сама быстро исчезать: внутренний возврат `SYS_CONSOLE_READ=2` игнорируется VGADRV.
5. Проверить 8 RT и затем 25 MT + 8 RT, после возврата из заставки выполнить `MTSTAT`, `MTSTAT WATCH`, `RTSTAT WATCH`, `MTSTOP ALL`.

## Что не менялось
Алгоритм RT/MT scheduler, RT priority, MT quantum 20 ms, ABI syscall 0..54, RTDATA/MTDATA, статистика, WATCH/ESC, kernel256 и FAT16 LBA512.
