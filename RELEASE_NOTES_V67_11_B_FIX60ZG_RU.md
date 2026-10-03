# ToyOS v67.11-B FIX60ZG

Исправлена starvation MT при одновременной работе постоянно готовых MT-сервисов и RT-задачи 10 ms, запущенной через RTD/F10. `rt_shell_hold` теперь потребляется scheduler-ом на первом настоящем Ring-3 PIT IRQ и даёт одно bounded MT/foreground окно без потери RT release. `TESTMRT.TST` переведён на реальный F10-сценарий 10/10 ms. Обновлён TOYOS_ARCHITECTURE_VISION.md.
