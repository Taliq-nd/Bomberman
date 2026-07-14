# test.ps1 — Build and run ASCII Bomberman (Windows / MSYS2)
# Usage: .\test.ps1

# Build first
& "$PSScriptRoot\make.ps1"

if ($LASTEXITCODE -eq 0) {
    Write-Host "Launching game..." -ForegroundColor Cyan
    & ".\bomberman.exe"
} else {
    Write-Host "Cannot run: build failed." -ForegroundColor Red
}
