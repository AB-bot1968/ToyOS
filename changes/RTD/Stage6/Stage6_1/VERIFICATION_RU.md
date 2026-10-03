# Stage 6.1 — Verification

## Source-level verification

- `RT_TIME_HZ=100` — present.
- `rt_time_ticks` increments on every IRQ0 — present.
- `timer_ticks` increments on every second IRQ0 — present.
- Detached RT scheduler is serviced on every hardware tick — present.
- Legacy EXECMT scheduler is serviced on logical 50 Hz ticks — present.
- `SYS_RT_TIME_GET=45` returns `rt_time_ticks` — present.
- RT release/deadline/wait paths use `rt_time_ticks` — present.

## Host verification

`check_rtd_stage6_1.sh` passed.

## Expected timing mapping

| Requested | RT ticks |
|---:|---:|
| 10 ms | 1 |
| 20 ms | 2 |
| 40 ms | 4 |
| 100 ms | 10 |

## Runtime limitation

The development environment used for this archive does not contain `qemu-system-i386`, therefore hardware boot/runtime for this step is not claimed as locally verified. The runtime test plan above must be performed on the target ToyOS environment.
