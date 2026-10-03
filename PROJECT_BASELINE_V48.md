# Toy OS — PROJECT BASELINE v48

v48 продолжает v47 без удаления существующих возможностей.

Исправлена причина отсутствия `Task NN` у `execmt`: начальные Ring-3
scheduler contexts теперь получают валидные DS/ES/FS/GS = 0x23.

Standalone EXECMT images по-прежнему загружаются в отдельные физические
1-MiB slots, получают отдельные CR3 и stack pages и вызывают
`SYS_CONSOLE_WRITE` для `Task NN\\n`.
