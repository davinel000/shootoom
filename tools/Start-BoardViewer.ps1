param([switch]$NoBrowser)

$ErrorActionPreference = 'Stop'
$editorPath = Join-Path $PSScriptRoot 'perfboard-editor'
$viteEntry = Join-Path $editorPath 'node_modules\vite\bin\vite.js'
$projectFile = 'shootoom-system-guide.html'
$baseUrl = 'http://127.0.0.1:4173/'
$projectUrl = $baseUrl + $projectFile

function Get-ViewerRunning {
    try {
        $page = Invoke-WebRequest -Uri $baseUrl -UseBasicParsing -TimeoutSec 2
        return $page.StatusCode -eq 200 -and $page.Content.Contains('Perfboard Designer')
    } catch { return $false }
}

try {
    if (-not (Get-ViewerRunning)) {
        $nodeExe = Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies\node\bin\node.exe'
        if (-not (Test-Path -LiteralPath $nodeExe)) {
            $nodeCommand = Get-Command node.exe -ErrorAction SilentlyContinue
            if (-not $nodeCommand) { throw 'Node.js was not found. Install Node.js or restore the Codex runtime.' }
            $nodeExe = $nodeCommand.Source
        }
        if (-not (Test-Path -LiteralPath $viteEntry)) {
            throw "Open tools/perfboard-editor and run npm ci --ignore-scripts, then launch again."
        }
        if (-not (Test-Path -LiteralPath (Join-Path $editorPath 'dist\index.html'))) {
            Push-Location -LiteralPath $editorPath
            try {
                & $nodeExe $viteEntry build
                if ($LASTEXITCODE -ne 0) { throw 'The editor build failed.' }
            } finally { Pop-Location }
        }
        $stdoutFile = Join-Path $env:TEMP 'shootoom-board-viewer.stdout.log'
        $stderrFile = Join-Path $env:TEMP 'shootoom-board-viewer.stderr.log'
        $arguments = '"{0}" preview --host 127.0.0.1 --port 4173 --strictPort' -f $viteEntry
        $server = Start-Process -FilePath $nodeExe -ArgumentList $arguments -WorkingDirectory $editorPath -WindowStyle Hidden -RedirectStandardOutput $stdoutFile -RedirectStandardError $stderrFile -PassThru
        $ready = $false
        for ($attempt = 0; $attempt -lt 20; $attempt++) {
            if (Get-ViewerRunning) { $ready = $true; break }
            if ($server.HasExited) { break }
            Start-Sleep -Milliseconds 500
        }
        if (-not $ready) { throw "Could not start the board viewer. Details: $stderrFile" }
    }
    $null = Invoke-WebRequest -Uri ($baseUrl + $projectFile) -UseBasicParsing -TimeoutSec 3
    if (-not $NoBrowser) { Start-Process -FilePath $projectUrl }
    Write-Host "Board viewer is ready: $projectUrl"
    Write-Host 'The server continues running in the background. You can close this window.'
    exit 0
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
