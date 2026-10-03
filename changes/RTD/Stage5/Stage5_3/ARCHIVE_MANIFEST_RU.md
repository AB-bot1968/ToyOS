# Stage 5.3 archive manifest

Source base: ToyOS v67 RTD Stage 5.2 FIX2 / subsequent verified Stage 5.2 runtime fixes.

Added:
- `src/rt_period_skip_diag.c`
- `tools/test_rt_period_release.c`
- `check_rtd_stage5_3.sh`
- `changes/RTD/Stage5/Stage5_3/*`

Modified:
- `src/kernel.c`
- `src/rt_deadline.h`
- `build.sh`

No `build/` directory is included in the source archive.

Full review patch: `STAGE5_3_FULL.patch`.
