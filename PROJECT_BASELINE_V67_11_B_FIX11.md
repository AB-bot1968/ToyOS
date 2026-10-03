# ToyOS v67.11-B-FIX11

Baseline: v67.11-B-FIX10.

RT scheduler corrections:
- Fixed `rt_next_release_after()`: when `now-candidate` is an exact multiple of the period, the release at `now` is no longer incorrectly skipped. The skipped count now uses ceil(delta/period), not floor(delta/period)+1.
- Fixed RT budget handling: when the current on-time RT job is the only READY RT job, expiry of its accounting budget no longer forces it to the shell. It continues until `SYS_RT_WAIT`; the shell resumes immediately after the job blocks.
- Preserved FIX9 strict deadline semantics and cumulative RTSTAT/DUMP MISS/SKIP snapshots.
- Removed generated `build/` directory from the source release.
