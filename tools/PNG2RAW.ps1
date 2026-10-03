# ПРИМЕЧАНИЕ v65.15: исторический резервный конвертер.
# Стандартная утилита Toy OS теперь tools\PNG2RAW.EXE, собранная GCC.
# Этот PowerShell-файл не используется build.sh и PNG2RAW.BAT.

<#
    PNG2RAW.PS1 — автономный конвертер PNG -> RAW для заставки Toy OS.

    ВАЖНО:
      Эта версия НЕ использует System.Drawing.Bitmap и GDI+.
      PNG разбирается непосредственно по стандартным PNG chunk-ам, данные IDAT
      распаковываются штатным .NET DeflateStream, затем применяются PNG-фильтры.

    Формат результата Toy OS:
      320 x 200 пикселей;
      8 bit/pixel;
      256 индексов палитры VGADRV;
      ровно 64000 байт без заголовка.

    Поддерживаемые PNG:
      - bit depth = 8;
      - color type 0 (grayscale);
      - color type 2 (RGB);
      - color type 3 (indexed palette, с/без tRNS);
      - color type 4 (grayscale + alpha);
      - color type 6 (RGBA).
      Для цветных изображений выполняется масштабирование до 320x200 методом
      ближайшего соседа. Альфа смешивается с чёрным фоном.

    Совместимость:
      Windows 7 + встроенный PowerShell/.NET, без сторонних DLL.

    Пример:
      PNG2RAW.BAT my_splash.png SPLASH.RAW
#>

param(
    [Parameter(Mandatory=$true, Position=0)] [string]$InputPng,
    [Parameter(Mandatory=$false, Position=1)] [string]$OutputRaw = "SPLASH.RAW"
)

$ErrorActionPreference = "Stop"

function Fail([string]$Message) {
    Write-Host "PNG2RAW: ERROR: $Message" -ForegroundColor Red
    exit 1
}

function Read-U32BE([byte[]]$Data, [ref]$Pos) {
    if ($Pos.Value + 4 -gt $Data.Length) { throw "unexpected end of PNG" }
    $v = ([uint32]$Data[$Pos.Value] -shl 24) -bor
         ([uint32]$Data[$Pos.Value + 1] -shl 16) -bor
         ([uint32]$Data[$Pos.Value + 2] -shl 8) -bor
         [uint32]$Data[$Pos.Value + 3]
    $Pos.Value += 4
    return $v
}

function Read-Bytes([byte[]]$Data, [ref]$Pos, [int]$Count) {
    if ($Count -lt 0 -or $Pos.Value + $Count -gt $Data.Length) { throw "unexpected end of PNG" }
    $out = New-Object byte[] $Count
    [Array]::Copy($Data, $Pos.Value, $out, 0, $Count)
    $Pos.Value += $Count
    return $out
}

function Paeth([int]$A, [int]$B, [int]$C) {
    $p = $A + $B - $C
    $pa = [Math]::Abs($p - $A)
    $pb = [Math]::Abs($p - $B)
    $pc = [Math]::Abs($p - $C)
    if ($pa -le $pb -and $pa -le $pc) { return $A }
    if ($pb -le $pc) { return $B }
    return $C
}

function Unfilter-PngRows([byte[]]$Raw, [int]$Width, [int]$Height, [int]$BytesPerPixel, [int]$RowBytes) {
    $expected = ($RowBytes + 1) * $Height
    if ($Raw.Length -ne $expected) {
        throw "invalid decompressed IDAT size: $($Raw.Length), expected $expected"
    }

    $pixels = New-Object byte[] ($RowBytes * $Height)
    $src = 0
    for ($y = 0; $y -lt $Height; $y++) {
        $filter = $Raw[$src]; $src++
        $rowBase = $y * $RowBytes
        $prevBase = ($y - 1) * $RowBytes

        for ($x = 0; $x -lt $RowBytes; $x++) {
            $rawByte = [int]$Raw[$src]; $src++
            $left = if ($x -ge $BytesPerPixel) { [int]$pixels[$rowBase + $x - $BytesPerPixel] } else { 0 }
            $up = if ($y -gt 0) { [int]$pixels[$prevBase + $x] } else { 0 }
            $upLeft = if ($y -gt 0 -and $x -ge $BytesPerPixel) { [int]$pixels[$prevBase + $x - $BytesPerPixel] } else { 0 }

            switch ($filter) {
                0 { $v = $rawByte }
                1 { $v = ($rawByte + $left) -band 0xFF }
                2 { $v = ($rawByte + $up) -band 0xFF }
                3 { $v = ($rawByte + [int](($left + $up) / 2)) -band 0xFF }
                4 { $v = ($rawByte + (Paeth $left $up $upLeft)) -band 0xFF }
                default { throw "unsupported PNG filter type: $filter" }
            }
            $pixels[$rowBase + $x] = [byte]$v
        }
    }
    return $pixels
}

function Inflate-Zlib([byte[]]$ZlibData) {
    if ($ZlibData.Length -lt 6) { throw "IDAT zlib stream is too short" }
    $input = New-Object System.IO.MemoryStream
    $input.Write($ZlibData, 0, $ZlibData.Length)
    $input.Position = 2 # пропускаем заголовок zlib; DeflateStream читает raw DEFLATE ниже.
    $deflate = New-Object System.IO.Compression.DeflateStream($input, [System.IO.Compression.CompressionMode]::Decompress)
    $output = New-Object System.IO.MemoryStream
    $buffer = New-Object byte[] 8192
    try {
        while ($true) {
            $count = $deflate.Read($buffer, 0, $buffer.Length)
            if ($count -le 0) { break }
            $output.Write($buffer, 0, $count)
        }
    }
    finally {
        $deflate.Dispose()
        $input.Dispose()
    }
    return $output.ToArray()
}

function Build-VgaPalette {
    $palette = New-Object 'System.Collections.Generic.List[object]'
    for ($r = 0; $r -lt 6; $r++) {
        for ($g = 0; $g -lt 6; $g++) {
            for ($b = 0; $b -lt 6; $b++) {
                $rr = [int](($r * 63) / 5)
                $gg = [int](($g * 63) / 5)
                $bb = [int](($b * 63) / 5)
                $palette.Add(@($rr,$gg,$bb))
            }
        }
    }
    for ($i = 0; $i -lt 40; $i++) {
        $v = [int](($i * 63) / 39)
        $palette.Add(@($v,$v,$v))
    }
    return $palette
}

function Rgb8ToVga6([int]$Value) {
    return [int](($Value * 63 + 127) / 255)
}

function Find-NearestVgaIndex([int]$R, [int]$G, [int]$B, $Palette) {
    $r6 = Rgb8ToVga6 $R
    $g6 = Rgb8ToVga6 $G
    $b6 = Rgb8ToVga6 $B
    $bestIndex = 0
    $bestDistance = [int64]::MaxValue
    for ($i = 0; $i -lt 256; $i++) {
        $p = $Palette[$i]
        $dr = $r6 - [int]$p[0]
        $dg = $g6 - [int]$p[1]
        $db = $b6 - [int]$p[2]
        $d = [int64]$dr*$dr + [int64]$dg*$dg + [int64]$db*$db
        if ($d -lt $bestDistance) {
            $bestDistance = $d
            $bestIndex = $i
            if ($d -eq 0) { break }
        }
    }
    return $bestIndex
}

function Read-PngRgba([byte[]]$Png) {
    $sig = @(137,80,78,71,13,10,26,10)
    if ($Png.Length -lt 8) { throw "file is too short" }
    for ($i = 0; $i -lt 8; $i++) {
        if ($Png[$i] -ne $sig[$i]) { throw "invalid PNG signature" }
    }

    $pos = 8
    $seenIHDR = $false
    $seenIEND = $false
    $width = 0; $height = 0; $bitDepth = 0; $colorType = 0
    $palette = $null
    $transparency = $null
    $idat = New-Object System.IO.MemoryStream

    try {
        while ($pos -lt $Png.Length) {
            $length = [int](Read-U32BE $Png ([ref]$pos))
            if ($length -lt 0) { throw "invalid PNG chunk length" }
            $typeBytes = Read-Bytes $Png ([ref]$pos) 4
            $type = [System.Text.Encoding]::ASCII.GetString($typeBytes)
            $chunk = Read-Bytes $Png ([ref]$pos) $length
            [void](Read-U32BE $Png ([ref]$pos)) # CRC; структура проверяется отдельно логикой PNG.

            switch ($type) {
                'IHDR' {
                    if ($seenIHDR -or $length -ne 13) { throw "invalid IHDR" }
                    $p2 = 0
                    $width = [int](Read-U32BE $chunk ([ref]$p2))
                    $height = [int](Read-U32BE $chunk ([ref]$p2))
                    $bitDepth = $chunk[$p2]; $p2++
                    $colorType = $chunk[$p2]; $p2++
                    $compression = $chunk[$p2]; $p2++
                    $filterMethod = $chunk[$p2]; $p2++
                    $interlace = $chunk[$p2]
                    if ($width -le 0 -or $height -le 0) { throw "invalid image dimensions" }
                    if ($bitDepth -ne 8) { throw "only 8-bit PNG is supported" }
                    if ($colorType -notin 0,2,3,4,6) { throw "unsupported PNG color type: $colorType" }
                    if ($compression -ne 0 -or $filterMethod -ne 0 -or $interlace -ne 0) {
                        throw "only non-interlaced PNG with standard compression/filter methods is supported"
                    }
                    $seenIHDR = $true
                }
                'PLTE' {
                    if ($length -eq 0 -or ($length % 3) -ne 0) { throw "invalid PLTE" }
                    $palette = New-Object 'System.Collections.Generic.List[object]'
                    for ($i = 0; $i -lt $length; $i += 3) { $palette.Add(@($chunk[$i],$chunk[$i+1],$chunk[$i+2])) }
                }
                'tRNS' {
                    $transparency = $chunk
                }
                'IDAT' {
                    $idat.Write($chunk, 0, $chunk.Length)
                }
                'IEND' {
                    if ($length -ne 0) { throw "invalid IEND" }
                    $seenIEND = $true
                    break
                }
            }
        }
    }
    finally {
        # The caller only needs the accumulated IDAT bytes; stream is disposed below.
    }

    if (-not $seenIHDR -or -not $seenIEND) { $idat.Dispose(); throw "PNG is missing IHDR or IEND" }
    if ($idat.Length -eq 0) { $idat.Dispose(); throw "PNG contains no IDAT data" }

    switch ($colorType) {
        0 { if ($transparency -and $transparency.Length -ne 2) { $idat.Dispose(); throw "invalid grayscale tRNS" } }
        2 { if ($transparency -and $transparency.Length -ne 6) { $idat.Dispose(); throw "invalid RGB tRNS" } }
        3 { if (-not $palette) { $idat.Dispose(); throw "indexed PNG requires PLTE" } }
        4 { }
        6 { }
    }

    $channels = @{0=1;2=3;3=1;4=2;6=4}[$colorType]
    $rowBytes = $width * $channels
    $raw = Inflate-Zlib $idat.ToArray()
    $idat.Dispose()
    $unfiltered = Unfilter-PngRows $raw $width $height $channels $rowBytes

    $rgba = New-Object byte[] ($width * $height * 4)
    $dst = 0
    for ($y = 0; $y -lt $height; $y++) {
        $row = $y * $rowBytes
        for ($x = 0; $x -lt $width; $x++) {
            $src = $row + $x * $channels
            switch ($colorType) {
                0 {
                    $g = [int]$unfiltered[$src]
                    $a = 255
                    if ($transparency) {
                        $transparentG = ([int]$transparency[0] -shl 8) -bor [int]$transparency[1]
                        if ($g -eq $transparentG) { $a = 0 }
                    }
                    $rgba[$dst]=$g; $rgba[$dst+1]=$g; $rgba[$dst+2]=$g; $rgba[$dst+3]=$a
                }
                2 {
                    $r=[int]$unfiltered[$src]; $g=[int]$unfiltered[$src+1]; $b=[int]$unfiltered[$src+2]; $a=255
                    if ($transparency) {
                        $tr=(([int]$transparency[0] -shl 8) -bor [int]$transparency[1])
                        $tg=(([int]$transparency[2] -shl 8) -bor [int]$transparency[3])
                        $tb=(([int]$transparency[4] -shl 8) -bor [int]$transparency[5])
                        if ($r -eq $tr -and $g -eq $tg -and $b -eq $tb) {$a=0}
                    }
                    $rgba[$dst]=$r; $rgba[$dst+1]=$g; $rgba[$dst+2]=$b; $rgba[$dst+3]=$a
                }
                3 {
                    $idx=[int]$unfiltered[$src]
                    if ($idx -ge $palette.Count) { throw "palette index out of range" }
                    $p=$palette[$idx]; $a=255
                    if ($transparency -and $idx -lt $transparency.Length) { $a=[int]$transparency[$idx] }
                    $rgba[$dst]=[byte]$p[0]; $rgba[$dst+1]=[byte]$p[1]; $rgba[$dst+2]=[byte]$p[2]; $rgba[$dst+3]=$a
                }
                4 {
                    $g=[int]$unfiltered[$src]; $a=[int]$unfiltered[$src+1]
                    $rgba[$dst]=$g; $rgba[$dst+1]=$g; $rgba[$dst+2]=$g; $rgba[$dst+3]=$a
                }
                6 {
                    $rgba[$dst]=$unfiltered[$src]; $rgba[$dst+1]=$unfiltered[$src+1]; $rgba[$dst+2]=$unfiltered[$src+2]; $rgba[$dst+3]=$unfiltered[$src+3]
                }
            }
            $dst += 4
        }
    }
    return @{Width=$width; Height=$height; Rgba=$rgba}
}

try {
    if (-not (Test-Path -LiteralPath $InputPng -PathType Leaf)) { Fail "input PNG not found: $InputPng" }
    $fullInput = [System.IO.Path]::GetFullPath((Resolve-Path -LiteralPath $InputPng))
    $fullOutput = [System.IO.Path]::GetFullPath($OutputRaw)
    if ([StringComparer]::OrdinalIgnoreCase.Equals($fullInput,$fullOutput)) { Fail "input and output must be different files" }

    $pngBytes = [System.IO.File]::ReadAllBytes($fullInput)
    $image = Read-PngRgba $pngBytes
    $srcW=$image.Width; $srcH=$image.Height; $rgba=$image.Rgba

    $palette = Build-VgaPalette
    $rawOut = New-Object byte[] 64000
    $o = 0
    for ($y = 0; $y -lt 200; $y++) {
        # nearest-neighbour без зависимости от GDI+: предсказуемо и одинаково на Win7.
        $sy = [int](($y * $srcH) / 200)
        if ($sy -ge $srcH) { $sy = $srcH - 1 }
        for ($x = 0; $x -lt 320; $x++) {
            $sx = [int](($x * $srcW) / 320)
            if ($sx -ge $srcW) { $sx = $srcW - 1 }
            $si = ($sy * $srcW + $sx) * 4
            $r=[int]$rgba[$si]; $g=[int]$rgba[$si+1]; $b=[int]$rgba[$si+2]; $a=[int]$rgba[$si+3]
            if ($a -lt 255) {
                # Alpha compositing на чёрный фон.
                $r = [int](($r * $a + 127) / 255)
                $g = [int](($g * $a + 127) / 255)
                $b = [int](($b * $a + 127) / 255)
            }
            $rawOut[$o] = [byte](Find-NearestVgaIndex $r $g $b $palette)
            $o++
        }
    }

    $outDir = [System.IO.Path]::GetDirectoryName($fullOutput)
    if ($outDir -and -not (Test-Path -LiteralPath $outDir -PathType Container)) { New-Item -ItemType Directory -Path $outDir | Out-Null }
    [System.IO.File]::WriteAllBytes($fullOutput,$rawOut)

    $size=(Get-Item -LiteralPath $fullOutput).Length
    if ($size -ne 64000) { Fail "invalid RAW size: $size (expected 64000)" }

    Write-Host "PNG2RAW: OK"
    Write-Host "Input : $fullInput"
    Write-Host "Output: $fullOutput"
    Write-Host ("Source PNG: {0}x{1}" -f $srcW,$srcH)
    Write-Host "Output RAW: 320x200, 256-color VGA, 64000 bytes"
    Write-Host "System.Drawing is not used."
    exit 0
}
catch {
    Fail $_.Exception.Message
}
