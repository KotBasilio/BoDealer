param(
    [Parameter(Mandatory = $true)]
    [string]$WalrusPath
)

$ErrorActionPreference = "Stop"

$walrus = (Resolve-Path $WalrusPath).Path
$testDirectory = Join-Path ([System.IO.Path]::GetTempPath()) "bodealer-regression"
$missingConfig = Join-Path $testDirectory "config-does-not-exist.txt"
$stdoutPath = Join-Path $testDirectory "walrus-stdout.txt"
$stderrPath = Join-Path $testDirectory "walrus-stderr.txt"

New-Item -ItemType Directory -Path $testDirectory -Force | Out-Null
Remove-Item $missingConfig -Force -ErrorAction SilentlyContinue

$startInfo = [System.Diagnostics.ProcessStartInfo]::new()
$startInfo.FileName = $walrus
$startInfo.Arguments = "-cfgname `"$missingConfig`" -exitondone"
$startInfo.WorkingDirectory = (Get-Location).Path
$startInfo.UseShellExecute = $false
$startInfo.RedirectStandardOutput = $true
$startInfo.RedirectStandardError = $true

$process = [System.Diagnostics.Process]::new()
$process.StartInfo = $startInfo

if (-not $process.Start()) {
    throw "Failed to start Walrus: $walrus"
}

$stdoutTask = $process.StandardOutput.ReadToEndAsync()
$stderrTask = $process.StandardError.ReadToEndAsync()

if (-not $process.WaitForExit(10000)) {
    $process.Kill()
    throw "Walrus did not exit within 10 seconds"
}

$stdout = $stdoutTask.Result
$stderr = $stderrTask.Result
$stdout | Set-Content $stdoutPath
$stderr | Set-Content $stderrPath

Write-Host $stdout
if ($stderr) {
    Write-Host "Walrus stderr:"
    Write-Host $stderr
}

if ($process.ExitCode -ne 0) {
    throw "Expected legacy Walrus exit code 0, got $($process.ExitCode)"
}
if ($stdout -notmatch "config-does-not-exist\.txt' not found") {
    throw "Missing expected not-found diagnostic"
}
if ($stdout -notmatch "Failed to init configuration") {
    throw "Missing expected configuration-failure diagnostic"
}

Write-Host "PASS: legacy Walrus reports a missing configuration and exits cleanly"
