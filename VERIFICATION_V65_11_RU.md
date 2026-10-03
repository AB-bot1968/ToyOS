# Verification v65.11

## Нижний prompt

Проверить `exec VGADRV.EXE SPLASH.RAW`, затем нажать любую клавишу. Ожидается
`VGADRV: OK` в строке 24 и `toy0>` в левом нижнем углу строки 25.

## AUTOSTART.SH

Поставляемый образ содержит `/AUTOSTART.SH`:

```text
exec VGADRV.EXE SPLASH.RAW
```

Для проверки построчного выполнения заменить ресурс, например, на:

```text
echo START
pwd
ls
exec VGADRV.EXE SPLASH.RAW
echo DONE
```

Для проверки обычного старта удалить `resources/AUTOSTART.SH` и пересобрать образ.
