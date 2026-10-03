# ToyOS v67.11-B FIX35B — RT fault/exit context preservation

Основа: FIX35A. Последняя runtime-подтвержденная стабильная точка отката: FIX34A.

FIX35A исправил определение владельца fault по CR3, но оставил вторую ошибку в teardown: после перевода faulting RT в TASK_EXIT `rt_exit_slot()` вызывал обычный `rt_switch_to()`. Обычный путь видел, что TASK_EXIT уже не является live RT, ошибочно принимал exception frame за foreground shell и записывал его в `rt_shell_frame`. Если существовала READY RT-задача более низкого приоритета, именно этот путь срабатывал; после ее SYS_RT_WAIT возврат шел в поврежденный "shell" frame. Это соответствует runtime-наблюдению: fault с самым низким приоритетом работал, а fault, вытесняющий SENSOR, оставлял `toy0>` и мигающий курсор, но shell не принимал команды.

FIX35B разделяет обычное переключение RT и hand-off от заведомо текущего завершающегося RT. В последнем случае запрещено сохранять exception/syscall frame как shell. Дополнительно при завершении RT выбирается normal READY, затем missed READY, прежде чем возвращаться к shell. Scheduler policy, сравнение приоритетов, release/deadline, MT quantum и ABI не изменены.

Исправление syscall-test из FIX35A сохранено без изменений.
