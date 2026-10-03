# Configure repository-local Git identity for the ToyOS FIX60ZEI release.
$ErrorActionPreference = 'Stop'
try {
    & git config user.name 'Ботнев Александр Валерьевич'
    if ($LASTEXITCODE -ne 0) { throw 'git config user.name failed' }
    & git config user.email '101033309+AB-bot1968@users.noreply.github.com'
    if ($LASTEXITCODE -ne 0) { throw 'git config user.email failed' }
    Write-Host 'Git release identity configured for AB-bot1968.'
    exit 0
}
catch {
    Write-Host ('ERROR: ' + $_.Exception.Message)
    exit 1
}
