Add-Type -AssemblyName System.Windows.Forms

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$targets = @{
    "start"   = Join-Path $scriptDir "start.exe"
    "main"    = Join-Path $scriptDir "main.exe"
    "effect"  = Join-Path $scriptDir "effect.exe"
    "effect2" = Join-Path $scriptDir "effect2.exe"
}

function Get-TargetProcs($exePath) {
    Get-Process -ErrorAction SilentlyContinue | Where-Object {
        try { $_.MainModule.FileName -eq $exePath } catch { $false }
    }
}

$running = @{}
foreach ($name in $targets.Keys) {
    $procs = Get-TargetProcs $targets[$name]
    if ($procs) { $running[$name] = $true }
}

while ($true) {
    Start-Sleep -Seconds 2

    foreach ($name in $targets.Keys) {
        $procs = Get-TargetProcs $targets[$name]
        $wasRunning = $running.ContainsKey($name) -and $running[$name]

        if ($wasRunning -and -not $procs) {
            $running[$name] = $false
            $result = [System.Windows.Forms.MessageBox]::Show(
                "$name 프로그램이 종료되었습니다. 재부팅하시겠습니까?",
                "재부팅 확인",
                [System.Windows.Forms.MessageBoxButtons]::YesNo,
                [System.Windows.Forms.MessageBoxIcon]::Warning
            )
            if ($result -eq [System.Windows.Forms.DialogResult]::Yes) {
                shutdown /r /t 0
                exit
            }
        }
        elseif ($procs) {
            $running[$name] = $true
        }
    }
}
