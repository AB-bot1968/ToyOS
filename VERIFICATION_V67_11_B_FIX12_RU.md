# Проверка v67.11-B FIX12

Статически проверено:
- SYS_RT_DATA имеет номер 50, существующие номера syscall не изменены;
- буфер каждого RT-слота: 255 байт данных + NUL;
- публикация возможна только текущей RT-задачей, слот выбирает ядро;
- snapshot очищается при START/reuse, STOP и EXIT;
- seq uint32_t допускает wrap-around; значение не выводится RTDATA;
- RTDATA SLOT1..SLOT4 регистронезависима через существующий eq();
- SENSOR1..4 не обращаются к console syscall и публикуют данные перед RT_WAIT;
- kernel.c, user_shell.c и rt_sensor_diag.c проходят host gcc -m32 freestanding compile (имеются только прежние предупреждения в kernel/shell).

Полная сборка build.sh в Linux-среде не заявляется: проект требует i686 W64DevKit target. Нужна runtime-проверка в штатной среде ToyOS, включая подтверждение, что FIX11 MISS/SKIP=0 не регрессировал.
