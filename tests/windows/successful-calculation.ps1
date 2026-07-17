param(
    [Parameter(Mandatory = $true)]
    [string]$WalrusPath
)

$ErrorActionPreference = "Stop"

$walrus = (Resolve-Path $WalrusPath).Path
$configPath = Join-Path $PSScriptRoot "configs/deterministic-grand-slam/start_from.txt"
$testDirectory = Join-Path ([System.IO.Path]::GetTempPath()) "bodealer-calculation-$([guid]::NewGuid())"
$stdoutPath = Join-Path $testDirectory "walrus-stdout.txt"
$stderrPath = Join-Path $testDirectory "walrus-stderr.txt"

if (-not (Test-Path $configPath -PathType Leaf)) {
    throw "Missing deterministic calculation fixture: $configPath"
}

$existingOscarIds = @(0) + @(
    Get-Process -Name "Oscar" -ErrorAction SilentlyContinue |
        Select-Object -ExpandProperty Id
)

New-Item -ItemType Directory -Path $testDirectory -Force | Out-Null

try {
    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $walrus
    $startInfo.Arguments = "-cfgname `"$configPath`" -exitondone"
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

    if (-not $process.WaitForExit(60000)) {
        $process.Kill($true)
        throw "Walrus did not complete the deterministic calculation within 60 seconds"
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
    if ($stdout -notmatch "Primary scorer \(spades, 13 tr\):") {
        throw "Missing expected 7S primary scorer diagnostic"
    }
    if ($stdout -notmatch "Processed: [1-9][0-9]* total\. East is on lead\. Goal is 13 tricks in spades\.") {
        throw "Missing expected completed-calculation summary"
    }
    if ($stdout -notmatch "Averages:\s+ideal = 1510, 7S = 1510, 6S = 1010\.\s+Chance to make = 100\.0%\.") {
        throw "Unexpected deterministic grand-slam result"
    }

    $deadline = [DateTime]::UtcNow.AddSeconds(5)
    do {
        $oscarProcesses = @(
            Get-Process -Name "Oscar" -ErrorAction SilentlyContinue |
                Where-Object { $_.Id -notin $existingOscarIds }
        )
        if ($oscarProcesses.Count -eq 0) {
            break
        }
        Start-Sleep -Milliseconds 100
    } while ([DateTime]::UtcNow -lt $deadline)

    if ($oscarProcesses.Count -ne 0) {
        $ids = ($oscarProcesses.Id -join ", ")
        $oscarProcesses | Stop-Process -Force -ErrorAction SilentlyContinue
        throw "Oscar did not exit after the deterministic calculation (process IDs: $ids)"
    }

    Write-Host "PASS: legacy Walrus produces the deterministic 7S calculation result"
}
finally {
    Remove-Item $testDirectory -Recurse -Force -ErrorAction SilentlyContinue
}
