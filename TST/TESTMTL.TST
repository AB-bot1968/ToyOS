# FIX60L: foreground console must remain schedulable after detached MT start.
PRINT FIX60L MT CONSOLE OWNERSHIP TEST
RUN mtstop ALL
RUN rtstat stop
RUN rtstat reset
RUN hwwd disarm
RUN safemode normal 0
ASSERT MT_ACTIVE 0
RUN execmt HBEATOK.EXE
ASSERT MT_ACTIVE 1
# These commands can execute only if foreground Ring3 regained control.
RUN ps
RUN mtstat
RUN mtstop ALL
ASSERT MT_ACTIVE 0
ASSERT PROC_HANDLES 0
PRINT FIX60L MT CONSOLE OWNERSHIP PASS
