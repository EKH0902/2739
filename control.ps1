Add-Type -AssemblyName System.Windows.Forms

$targets = @("start", "main", "effect", "effect2")

$running = @{}
foreach ($name in $targets) {
    $procs = Get-Process -Name $name -ErrorAction SilentlyContinue
    if ($procs) { $running[$name] = $true }
}

while ($true) {
    Start-Sleep -Seconds 2

    foreach ($name in $targets) {
        $procs = Get-Process -Name $name -ErrorAction SilentlyContinue
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
