# Release Notes — ToyOS v67.11-B-FIX3

База: v67.11-B-FIX2.

Исправлены две независимые ошибки фоновых RT-задач: unconditional SENSOR console
output и неправильный возврат RT-задачи после `SYS_RT_WAIT`, когда foreground shell
находился в `rtstat watch`. Также watch переведён на RT clock с обновлением 100 ms.

Полная загрузка в QEMU в данном окружении не выполнялась: отсутствуют QEMU и
полный i386/W64DevKit toolchain. Выполнены freestanding i386 syntax checks с
`-Wall -Wextra -Werror` и статические проверки исходников.
