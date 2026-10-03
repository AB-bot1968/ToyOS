# ToyOS v67.11-B FIX51 baseline

Parent stable baseline: ToyOS v67.11-B FIX50 Recovery Manager.

FIX51 scope: managed process recovery lifecycle only. Existing syscall numbering is preserved; SYS_RECOVERY_MANAGER remains 68 and FIX50 operations 0..2 remain compatible. Port I/O implementation and COMDRV/VGADRV/NETDRV are unchanged. RT/MT scheduler policy is unchanged.

Primary runtime acceptance: TESTRST.TST. Full previous regression suite remains mandatory before declaring FIX51 stable.
