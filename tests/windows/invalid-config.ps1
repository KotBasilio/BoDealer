param(
    [Parameter(Mandatory = $true)]
    [string]$WalrusPath
)

$ErrorActionPreference = "Stop"

$walrus = (Resolve-Path $WalrusPath).Path
$testDirectory = Join-Path ([System.IO.Path]::GetTempPath()) "bodealer-regression-$([guid]::NewGuid())"
$fixturesDirectory = Join-Path $PSScriptRoot "configs"

$cases = @(
    @{
        Name = "no selected task"
        Fixture = "no-selected-task"
        Expected = "Error: No task in selected in the config file"
    },
    @{
        Name = "unknown task"
        Fixture = "unknown-task"
        Expected = "Error: Task 'REGRESSION_UNKNOWN_TASK' not found in the config file"
    },
    @{
        Name = "malformed hand"
        Fixture = "malformed-hand"
        Expected = "Error: pls put fixed hand on NORTH, N. Your line is: [INVALID]"
    },
    @{
        Name = "invalid scorer"
        Fixture = "invalid-scorer"
        Expected = "Failed to parse prima scorer"
    },
    @{
        Name = "invalid filter"
        Fixture = "invalid-filter"
        Expected = "Unrecognized filter name DefinitelyNotAFilter"
    },
    @{
        Name = "missing task type"
        Fixture = "missing-task-type"
        Expected = "Error: 'TASK_TYPE:' line is missing."
    }
)

function Get-NewOscarProcesses {
    param([int[]]$ExistingIds)

    return @(
        Get-Process -Name "Oscar" -ErrorAction SilentlyContinue |
            Where-Object { $_.Id -notin $ExistingIds }
    )
}

function Wait-ForOscarExit {
    param(
        [int[]]$ExistingIds,
        [string]$CaseName
    )

    $deadline = [DateTime]::UtcNow.AddSeconds(5)
    do {
        $oscarProcesses = @(Get-NewOscarProcesses -ExistingIds $ExistingIds)
        if ($oscarProcesses.Count -eq 0) {
            return
        }
        Start-Sleep -Milliseconds 100
    } while ([DateTime]::UtcNow -lt $deadline)

    $ids = ($oscarProcesses.Id -join ", ")
    $oscarProcesses | Stop-Process -Force -ErrorAction SilentlyContinue
    throw "Oscar did not exit after case '$CaseName' (process IDs: $ids)"
}

New-Item -ItemType Directory -Path $testDirectory -Force | Out-Null

try {
    foreach ($case in $cases) {
        $safeName = $case.Name -replace "[^a-z0-9]+", "-"
        $caseDirectory = Join-Path $testDirectory $safeName
        $configPath = Join-Path (Join-Path $fixturesDirectory $case.Fixture) "start_from.txt"
        $stdoutPath = Join-Path $caseDirectory "walrus-stdout.txt"
        $stderrPath = Join-Path $caseDirectory "walrus-stderr.txt"
        New-Item -ItemType Directory -Path $caseDirectory -Force | Out-Null

        if (-not (Test-Path $configPath -PathType Leaf)) {
            throw "Missing configuration fixture for case '$($case.Name)': $configPath"
        }

        $existingOscarIds = @(0) + @(
            Get-Process -Name "Oscar" -ErrorAction SilentlyContinue |
                Select-Object -ExpandProperty Id
        )

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
            throw "Failed to start Walrus for case '$($case.Name)': $walrus"
        }

        $stdoutTask = $process.StandardOutput.ReadToEndAsync()
        $stderrTask = $process.StandardError.ReadToEndAsync()

        if (-not $process.WaitForExit(10000)) {
            $process.Kill($true)
            throw "Walrus did not exit within 10 seconds for case '$($case.Name)'"
        }

        $stdout = $stdoutTask.Result
        $stderr = $stderrTask.Result
        $stdout | Set-Content $stdoutPath
        $stderr | Set-Content $stderrPath

        Write-Host "Walrus output for $($case.Name):"
        Write-Host $stdout
        if ($stderr) {
            Write-Host "Walrus stderr for $($case.Name):"
            Write-Host $stderr
        }

        if ($process.ExitCode -ne 0) {
            throw "Expected legacy Walrus exit code 0 for case '$($case.Name)', got $($process.ExitCode)"
        }
        if ($stdout -notmatch [regex]::Escape($case.Expected)) {
            throw "Missing expected diagnostic for case '$($case.Name)': $($case.Expected)"
        }
        if ($stdout -notmatch "Failed to init configuration") {
            throw "Missing configuration-failure diagnostic for case '$($case.Name)'"
        }

        Wait-ForOscarExit -ExistingIds $existingOscarIds -CaseName $case.Name
        Write-Host "PASS: legacy Walrus rejects $($case.Name) and exits cleanly"
    }
}
finally {
    Remove-Item $testDirectory -Recurse -Force -ErrorAction SilentlyContinue
}
