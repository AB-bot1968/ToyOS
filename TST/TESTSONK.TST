# FIX60K: real SONARDRV image may be prepared as detached MT without hanging shell.
# No physical COM peer is required: the driver may timeout/retry in background;
# the test proves foreground commands remain executable and can stop the session.
PRINT FIX60K SONARDRV DETACHED START TEST
RUN mtstop ALL
RUN rtstat stop
RUN rtstat reset
RUN hwwd disarm
RUN safemode normal 0
ASSERT FILE_EXISTS /SONARDRV.EXE
RUN execmt SONARDRV.EXE
ASSERT MT_ACTIVE 1
RUN mtstat
RUN ps
RUN mtstop ALL
ASSERT MT_ACTIVE 0
ASSERT PROC_HANDLES 0
PRINT FIX60K SONARDRV DETACHED START PASS
