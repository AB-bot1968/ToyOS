# ToyOS FIX60ZEI Windows-native publication preflight.
# Compatible with Windows PowerShell 2.0+; does not require sha256sum.
$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $Root

function Fail([string]$Message) {
    Write-Host ("PREFLIGHT FAIL: " + $Message)
    exit 1
}
function Warn([string]$Message) {
    Write-Host ("PREFLIGHT WARNING: " + $Message)
}
function Pass([string]$Message) {
    Write-Host ("PREFLIGHT PASS: " + $Message)
}
function Get-Sha256([string]$Path) {
    $sha = New-Object System.Security.Cryptography.SHA256Managed
    $stream = [System.IO.File]::OpenRead($Path)
    try {
        $bytes = $sha.ComputeHash($stream)
        return ([System.BitConverter]::ToString($bytes).Replace('-', '').ToLowerInvariant())
    }
    finally {
        $stream.Dispose()
        $sha.Dispose()
    }
}
function Read-Utf8([string]$Path) {
    return [System.IO.File]::ReadAllText($Path, (New-Object System.Text.UTF8Encoding($false)))
}

try {
    $envPath = Join-Path $Root 'GIT_RELEASE.env'
    if (-not (Test-Path -LiteralPath $envPath -PathType Leaf)) { Fail 'GIT_RELEASE.env is missing' }
    $meta = Read-Utf8 $envPath
    if ($meta -notmatch "TOYOS_RELEASE_NAME='ToyOS v67\.11-B FIX60ZEI'") { Fail 'unexpected release metadata' }
    if ($meta -notmatch "TOYOS_GITHUB_OWNER='AB-bot1968'") { Fail 'unexpected GitHub owner' }
    if ($meta -notmatch "TOYOS_GITHUB_REPO='AB-bot1968/ToyOS'") { Fail 'unexpected GitHub repository' }
    if ($meta -notmatch "TOYOS_AUTHOR_NAME='Ботнев Александр Валерьевич'") { Fail 'unexpected author metadata' }

    $versionPath = Join-Path $Root 'VERSION.txt'
    if (-not (Test-Path -LiteralPath $versionPath -PathType Leaf)) { Fail 'VERSION.txt is missing' }
    $versionText = Read-Utf8 $versionPath
    if ($versionText -notmatch '^ToyOS v67\.11-B FIX60ZEI ') { Fail 'unexpected VERSION.txt' }

    $copyrightPath = Join-Path $Root 'COPYRIGHT'
    if (-not (Test-Path -LiteralPath $copyrightPath -PathType Leaf)) { Fail 'COPYRIGHT is missing' }
    $copyrightText = Read-Utf8 $copyrightPath
    if ($copyrightText -notmatch [regex]::Escape('Copyright © 2026 Ботнев Александр Валерьевич')) { Fail 'copyright holder mismatch' }
    if ($copyrightText -notmatch [regex]::Escape('All rights reserved.')) { Fail 'copyright reservation missing' }
    Pass 'copyright metadata verified'

    $attrPath = Join-Path $Root '.gitattributes'
    if (-not (Test-Path -LiteralPath $attrPath -PathType Leaf)) { Fail '.gitattributes is missing' }
    $attrText = Read-Utf8 $attrPath
    if ($attrText -notmatch '(?m)^\* -text\s*$') { Fail '.gitattributes must preserve exact bytes with * -text' }
    if ($attrText -match 'eol=') { Fail '.gitattributes must not force EOL conversion' }
    Pass 'Git attributes preserve exact frozen bytes'

    $manifestPath = Join-Path $Root 'FIX60ZEI_FROZEN_CONTENT_SHA256.txt'
    if (-not (Test-Path -LiteralPath $manifestPath -PathType Leaf)) { Fail 'frozen-content manifest is missing' }
    $manifestLines = [System.IO.File]::ReadAllLines($manifestPath, (New-Object System.Text.UTF8Encoding($false)))
    $checked = 0
    foreach ($line in $manifestLines) {
        if ($line.Trim().Length -eq 0) { continue }
        $m = [regex]::Match($line, '^([0-9A-Fa-f]{64})  (.+)$')
        if (-not $m.Success) { Fail ('invalid manifest line: ' + $line) }
        $expected = $m.Groups[1].Value.ToLowerInvariant()
        $relative = $m.Groups[2].Value
        if ($relative.StartsWith('./')) { $relative = $relative.Substring(2) }
        $relative = $relative.Replace('/', [System.IO.Path]::DirectorySeparatorChar)
        $full = Join-Path $Root $relative
        if (-not (Test-Path -LiteralPath $full -PathType Leaf)) { Fail ('manifest file missing: ' + $m.Groups[2].Value) }
        $actual = Get-Sha256 $full
        if ($actual -ne $expected) { Fail ('frozen FIX60ZEI content changed: ' + $m.Groups[2].Value) }
        $checked++
    }
    if ($checked -ne 1109) { Fail ('expected 1109 frozen manifest files, got ' + $checked) }
    Pass ('frozen FIX60ZEI manifest verified: ' + $checked + ' files')

    $kernel = Get-Sha256 (Join-Path $Root 'src\kernel.c')
    if ($kernel -ne '3af4e5b2a9c8610eaee3efb1b533836dadf843e1b212922c9fee354c860dd001') { Fail 'src/kernel.c differs from stable baseline' }
    $testmrt = Get-Sha256 (Join-Path $Root 'TST\TESTMRT.TST')
    if ($testmrt -ne '8bff965aa737de277a2bc901456508ff0199ae1ec350c85196af7f4f78302985') { Fail 'TST/TESTMRT.TST differs from stable baseline' }
    Pass 'critical runtime/test hashes match FIX60ZEI'

    if (Test-Path -LiteralPath (Join-Path $Root 'build')) { Fail 'build/ must not be committed' }
    if (Test-Path -LiteralPath (Join-Path $Root 'dist')) { Fail 'dist/ must not be committed' }
    if (Test-Path -LiteralPath (Join-Path $Root 'TST.LOG')) { Fail 'TST.LOG is forbidden' }
    $bTests = @(Get-ChildItem -LiteralPath (Join-Path $Root 'TST') -ErrorAction Stop | Where-Object { -not $_.PSIsContainer -and $_.Name -like 'B*.TST' })
    if ($bTests.Count -gt 0) { Fail ('B*.TST file is forbidden: ' + $bTests[0].Name) }

    $splash = Join-Path $Root 'resources\SPLASH.RAW'
    if (-not (Test-Path -LiteralPath $splash -PathType Leaf)) { Fail 'resources/SPLASH.RAW is missing' }
    if ((Get-Item -LiteralPath $splash).Length -ne 64000) { Fail 'resources/SPLASH.RAW must be 64000 bytes' }
    $testBin = Join-Path $Root 'tools\Python\TEST.BIN'
    if (-not (Test-Path -LiteralPath $testBin -PathType Leaf)) { Fail 'tools/Python/TEST.BIN fixture is missing' }
    $ignoreText = Read-Utf8 (Join-Path $Root '.gitignore')
    if ($ignoreText -notmatch '(?m)^!resources/SPLASH\.RAW\s*$') { Fail 'SPLASH.RAW is not unignored' }
    if ($ignoreText -notmatch '(?m)^!tools/Python/TEST\.BIN\s*$') { Fail 'TEST.BIN is not unignored' }
    Pass 'required binary resources are publication-safe'

    $acceptPath = Join-Path $Root 'TST\ACCEPT.TXT'
    if (-not (Test-Path -LiteralPath $acceptPath -PathType Leaf)) { Fail 'TST/ACCEPT.TXT is missing' }
    $names = New-Object System.Collections.ArrayList
    foreach ($raw in [System.IO.File]::ReadAllLines($acceptPath, (New-Object System.Text.UTF8Encoding($false)))) {
        $name = $raw.Trim()
        if ($name.Length -eq 0 -or $name.StartsWith('#')) { continue }
        if (-not (Test-Path -LiteralPath (Join-Path $Root ('TST\' + $name)) -PathType Leaf)) { Fail ('acceptance file missing: TST/' + $name) }
        [void]$names.Add($name)
    }
    if ($names.Count -ne 45) { Fail ('expected 45 acceptance tests, got ' + $names.Count) }
    $seen = @{}
    foreach ($name in $names) {
        $key = $name.ToUpperInvariant()
        if ($seen.ContainsKey($key)) { Fail ('duplicate acceptance test: ' + $name) }
        $seen[$key] = $true
    }
    Pass '45 unique acceptance tests are present'

    $badCredentialPatterns = @(
        'github_pat_[A-Za-z0-9_]{20,}',
        'ghp_[A-Za-z0-9]{20,}',
        '-----BEGIN ([A-Z ]+ )?PRIVATE KEY-----',
        'AKIA[0-9A-Z]{16}',
        'xox[baprs]-[A-Za-z0-9-]{20,}'
    )
    $textExt = @('.md','.txt','.c','.h','.sh','.bat','.ps1','.py','.ld','.tst','.env','.yml','.yaml','.json','.ini','.cfg')
    $allFiles = Get-ChildItem -LiteralPath $Root -Recurse -Force | Where-Object { -not $_.PSIsContainer }
    foreach ($f in $allFiles) {
        if ($f.FullName -match '[\\/]\.git[\\/]') { continue }
        if ($f.Length -gt 52428800) { Fail ('file larger than 50 MiB: ' + $f.FullName.Substring($Root.Length + 1)) }
        if ($textExt -contains $f.Extension.ToLowerInvariant()) {
            try { $text = [System.IO.File]::ReadAllText($f.FullName) } catch { continue }
            foreach ($pat in $badCredentialPatterns) {
                if ([regex]::IsMatch($text, $pat)) { Fail ('possible credential/token signature found in: ' + $f.FullName.Substring($Root.Length + 1)) }
            }
        }
    }
    Pass 'no common credential signatures or oversized files detected'

    $gitCmd = Get-Command git.exe -ErrorAction SilentlyContinue
    if ($gitCmd -and (Test-Path -LiteralPath (Join-Path $Root '.git'))) {
        & git diff --check
        if ($LASTEXITCODE -ne 0) { Fail 'git diff --check failed' }
    }

    if (-not (Test-Path -LiteralPath (Join-Path $Root 'LICENSE')) -and
        -not (Test-Path -LiteralPath (Join-Path $Root 'LICENSE.txt')) -and
        -not (Test-Path -LiteralPath (Join-Path $Root 'LICENSE.md'))) {
        Warn 'LICENSE is not selected; see LICENSE_STATUS_RU.md'
    }

    Write-Host 'PREFLIGHT OK: ToyOS v67.11-B FIX60ZEI is ready for Git commit/tag publication'
    exit 0
}
catch {
    Fail $_.Exception.Message
}
