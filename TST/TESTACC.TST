# FIX49 exhaustive automated acceptance regression.
# Replaces the normal manual MT/RT/recovery acceptance pass with structured checks.
PRINT FIX49 AUTOMATED ACCEPTANCE TEST
RUN mtstop ALL
RUN rtstat stop
RUN rtstat reset
RUN hwwd disarm
RUN safemode normal 0
RUN eventlog clear
ASSERT SAFE_MODE 0
ASSERT MT_ACTIVE 0
ASSERT RT_ACTIVE 0
ASSERT PROC_HANDLES 0
ASSERT HWWD_ARMED 0
ASSERT HEALTH_STATE 0

# MT lifecycle and case-insensitive shell dispatch.
RUN ExEcMt MT01.EXE MT02.EXE MT03.EXE MT04.EXE
ASSERT MT_ACTIVE 4
RUN MtStAt
RUN mTsToP aLl
ASSERT MT_ACTIVE 0
ASSERT PROC_HANDLES 0

# RT scheduler progress/history is tested independently from SLOT stop semantics.
# One task is sufficient to prove a real completed RT job and DUMP history.
START RT 1
ASSERT RT_ACTIVE 1
ASSERT RT_SLOT_PROGRESS 1
ASSERT RT_DUMP_NONEMPTY 1
RUN RtStAt DuMp
RUN rtstat stop
ASSERT RT_ACTIVE 0
RUN rtstat reset
ASSERT RT_ACTIVE 0
ASSERT PROC_HANDLES 0

# Named SLOT stop semantics need one indexed-stop proof; rt_stop_slot() is one
# common indexed implementation for SLOT1..SLOT8.  Capacity/all-slot cleanup
# are independently covered by TESTLOAD and the generic stop below.
START RT 4
ASSERT RT_ACTIVE 4
RUN rTsTaT sToP sLoT4
ASSERT RT_ACTIVE 3
RUN rtstat stop
ASSERT RT_ACTIVE 0
RUN rtstat reset
ASSERT RT_ACTIVE 0
ASSERT PROC_HANDLES 0

# Controlled supervisor fault -> restart budget -> SAFE chain.
RUN SaFeMoDe NoRmAl 0
RUN eventlog clear
RUN SuPsTaRt FAULTUD.EXE
WAIT LAST
ASSERT LAST_REASON 1
ASSERT LAST_STATUS 3
ASSERT SAFE_MODE 2
ASSERT SAFE_REASON 4591
ASSERT EVENT_REASON 2
ASSERT EVENT_TYPE 101
ASSERT EVENT_TYPE 103
ASSERT EVENT_TYPE 105
ASSERT EVENT_TYPE 102
ASSERT RECOVERY_FAULT_CHAIN
ASSERT EVENT_SEQ_MONOTONIC
ASSERT EVENT_TICK_MONOTONIC
RUN ReCoVeRyLoG

# Persistent copy must preserve ordered recovery history.
RUN EvEnTlOg SaVe
ASSERT EVENT_DISK_REASON 4591
ASSERT EVENT_DISK_SEQ_MONOTONIC
ASSERT EVENT_DISK_RECOVERY_CHAIN
RUN ReCoVeRyLoG DiSk

# Operator recovery and final cleanliness.
RUN mtstop ALL
RUN hwwd disarm
RUN safemode normal 0
RUN rtstat stop
RUN rtstat reset
ASSERT SAFE_MODE 0
ASSERT SAFE_REASON 0
ASSERT MT_ACTIVE 0
ASSERT RT_ACTIVE 0
ASSERT PROC_HANDLES 0
ASSERT HWWD_ARMED 0
ASSERT HEALTH_STATE 0
ASSERT HEALTH_RT 0
ASSERT HEALTH_MT 0
ASSERT HEALTH_HANDLES 0
PRINT FIX49 AUTOMATED ACCEPTANCE TEST COMPLETE
