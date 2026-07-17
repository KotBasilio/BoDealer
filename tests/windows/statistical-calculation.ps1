param(
    [Parameter(Mandatory = $true)]
    [string]$WalrusPath
)

$ErrorActionPreference = "Stop"

$walrus = (Resolve-Path $WalrusPath).Path
$configPath = Join-Path $PSScriptRoot "configs/seven-d-or-seven-nt/start_from.txt"
$testDirectory = Join-Path ([System.IO.Path]::GetTempPath()) "bodealer-statistics-$([guid]::NewGuid())"
$stdoutPath = Join-Path $testDirectory "walrus-stdout.txt"
$stderrPath = Join-Path $testDirectory "walrus-stderr.txt"
$resultPath = Join-Path $testDirectory "oscar-result.txt"

if (-not (Test-Path $configPath -PathType Leaf)) {
    throw "Missing 7D/7NT calculation fixture: $configPath"
}

$existingOscarIds = @(0) + @(
    Get-Process -Name "Oscar" -ErrorAction SilentlyContinue |
        Select-Object -ExpandProperty Id
)

New-Item -ItemType Directory -Path $testDirectory -Force | Out-Null

try {
    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $walrus
    $startInfo.Arguments = "-cfgname `"$configPath`" -logresult `"$resultPath`" -exitondone"
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
        throw "Walrus did not complete the 7D/7NT calculation within 60 seconds"
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
        throw "Oscar did not exit after the 7D/7NT calculation (process IDs: $ids)"
    }

    if (-not (Test-Path $resultPath -PathType Leaf)) {
        throw "Oscar did not create the 7D/7NT calculation result file"
    }
    $result = Get-Content $resultPath -Raw
    Write-Host "Oscar result:"
    Write-Host $result

    if ($process.ExitCode -ne 0) {
        throw "Expected legacy Walrus exit code 0, got $($process.ExitCode)"
    }
    if ($result -notmatch "Primary scorer \(diamonds, 13 tr\):") {
        throw "Missing expected 7D primary scorer diagnostic"
    }
    if ($result -notmatch "Contract-B scorer \(notrump, 13 tr\):") {
        throw "Missing expected 7NT secondary scorer diagnostic"
    }
    if ($result -notmatch "Processed: [1-9][0-9]* total\. West is on lead\. Goal is 13 tricks in diamonds\.") {
        throw "Missing expected completed 7D/7NT calculation summary"
    }

    $primaryResult = [regex]::Match(
        $result,
        "Averages:\s+7D = (?<average>-?[0-9]+)\.\s+Chance to make = (?<make>[0-9]+\.[0-9])%\."
    )
    $secondaryResult = [regex]::Match(
        $result,
        "7N: avg = (?<average>-?[0-9]+); makes in\s+(?<make>[0-9]+\.[0-9])% cases"
    )
    if (-not $primaryResult.Success -or -not $secondaryResult.Success) {
        throw "Missing expected 7D/7NT result fields"
    }

    $primaryAverage = [int]$primaryResult.Groups["average"].Value
    $secondaryAverage = [int]$secondaryResult.Groups["average"].Value
    $primaryMake = [double]::Parse(
        $primaryResult.Groups["make"].Value,
        [Globalization.CultureInfo]::InvariantCulture
    )
    $secondaryMake = [double]::Parse(
        $secondaryResult.Groups["make"].Value,
        [Globalization.CultureInfo]::InvariantCulture
    )

    if ($primaryAverage -le $secondaryAverage) {
        throw "Expected 7D average ($primaryAverage) to exceed 7NT average ($secondaryAverage)"
    }
    if ($primaryMake -le $secondaryMake) {
        throw "Expected 7D make percentage ($primaryMake) to exceed 7NT make percentage ($secondaryMake)"
    }

    Write-Host "PASS: legacy Walrus calculates 7D as stronger than 7NT on the recovered task"
}
finally {
    Remove-Item $testDirectory -Recurse -Force -ErrorAction SilentlyContinue
}
