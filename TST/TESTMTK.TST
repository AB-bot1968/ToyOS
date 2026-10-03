# FIX60K: EXECMT must return to foreground before first background dispatch.
PRINT FIX60K EXECMT FOREGROUND RETURN TEST
RUN mtstop ALL
RUN rtstat stop
RUN rtstat reset
RUN hwwd disarm
RUN safemode normal 0
ASSERT MT_ACTIVE 0
RUN execmt HBEATOK.EXE
# If SYS_EXECMT or its first dispatch steals the foreground continuation,
# TEST never reaches these assertions/commands.
ASSERT MT_ACTIVE 1
RUN mtstat
RUN ps
RUN mtstop ALL
ASSERT MT_ACTIVE 0
ASSERT PROC_HANDLES 0
PRINT FIX60K EXECMT FOREGROUND RETURN PASS
