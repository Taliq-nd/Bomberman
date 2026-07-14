# make.ps1 — Build script for ASCII Bomberman (Windows / MSYS2)
# Usage: .\make.ps1

$RAYLIB_INC = "C:\msys64\mingw64\include"
$RAYLIB_LIB = "C:\msys64\mingw64\lib"
$OUT        = "bomberman.exe"
$SRC        = Get-ChildItem -Path "src" -Filter "*.c" | ForEach-Object { $_.FullName }

Write-Host "Building ASCII Bomberman..." -ForegroundColor Cyan

$args_list = @(
    $SRC
    "-o", $OUT
    "-Isrc"
    "-I$RAYLIB_INC"
    "-L$RAYLIB_LIB"
    "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm"
    "-std=c99"
    "-Wall"
)

& gcc @args_list

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful! -> $OUT" -ForegroundColor Green
} else {
    Write-Host "Build FAILED." -ForegroundColor Red
    exit 1
}
