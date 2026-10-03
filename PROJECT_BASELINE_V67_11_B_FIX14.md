# ToyOS v67.11-B FIX14 — MTLIST

Base: FIX13 RT/MT separation.

- Added case-insensitive `MTLIST` shell command.
- `MTLIST` prints only currently live background MT tasks as `MTn NAME.EXE`.
- READY, RUNNING and BLOCKED are considered live; CREATE, STOPPED and EXIT are omitted.
- If no MT tasks are live, output is `MT: no tasks`.
- User-facing `MTSTAT` command/help is removed for now as agreed.
- Kernel syscall 51 snapshot ABI is retained internally; no syscall renumbering or ABI break.
- RT scheduler, RTDATA, MT round-robin policy, 20 ms quantum and MTSTOP behavior are unchanged.
- MTDATA is intentionally not implemented in this release.

Runtime validation in W64DevKit/QEMU is required before promoting FIX14 to stable.
