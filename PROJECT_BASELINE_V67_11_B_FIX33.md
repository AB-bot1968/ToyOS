# ToyOS v67.11-B FIX33 — EXE1 Lifecycle foundation

Base: runtime-confirmed FIX32 VGA RT/MT.

FIX33 adds an observational process-identity layer only. It does not replace or alter the existing foreground, MT, or RT schedulers.

## Added
- 32-bit monotonic process PID allocator (0 is never a valid PID; wrap is handled by skipping 0).
- Foreground EXE metadata: PID, type FG, lifecycle state, CR3, name.
- MT and RT task records carry an observational `process_pid` field. PID is assigned after the existing task context constructor, so scheduler initialization cannot erase it.
- New read-only syscall `SYS_PROCESS_INFO = 55`.
- New case-insensitive shell command `ps`.
- `ps` reports PID, TYPE, STATE, SLOT and NAME. MT slot and RT slot remain scheduler locations; PID identifies a particular loaded instance.

## SYS_PROCESS_INFO ABI
EBX = pointer to writable buffer for 34 records x 10 uint32_t. Return EAX = number of records, or 0xffffffff on invalid call.
Each record: PID, TYPE (1 FG / 2 MT / 3 RT), STATE (existing TASK_* numeric state), SLOT (0xffffffff for FG, zero-based for MT/RT), CR3, CURRENT, packed NAME[16] in four uint32_t words.

This syscall is diagnostic/read-only. Scheduler selection, RT priority/deadline/release accounting, MT 20 ms quantum, context return paths and stop/exit policy do not read PID metadata.

## Preserved
FIX32 VGA behavior; RT8; MT25; RT priority; MT 20 ms quantum; RT/MT data and statistics; WATCH/ESC; FIX31 console-wait MT progress; kernel area 256 sectors; FAT16 starts at LBA 512; syscall ABI 0..54 unchanged, with 55 appended.
