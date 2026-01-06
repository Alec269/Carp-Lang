# --- Configuration ---
$ConfigPreset = 'VsNinja'
$BuildPreset = 'VsNinja-debug'
$ExecutablePath = 'D:\1.Programe\C-Cpp\Compilers\Crap\out\build\bin\crap-comp.exe'
$TestFile = 'tests\main.carp'

# 1. Configure the project
Write-Host "--- Configuring CMake with preset: $ConfigPreset ---" -ForegroundColor Cyan
cmake --preset $ConfigPreset

if ($LASTEXITCODE -ne 0) {
   Write-Error 'CMake configuration failed.'
   exit $LASTEXITCODE
}

# 2. Build the project using the Build Preset
Write-Host "--- Building project with preset: $BuildPreset ---" -ForegroundColor Cyan
cmake --build --preset $BuildPreset

if ($LASTEXITCODE -ne 0) {
   Write-Error 'Build failed.'
   exit $LASTEXITCODE
}

# 3. Run the test
if (Test-Path $ExecutablePath) {
   Write-Host "--- Running Test: $TestFile ---" -ForegroundColor Green
   Write-Host '----------------------------------------------' -ForegroundColor Yellow
   & $ExecutablePath $TestFile
}
else {
   Write-Error "Executable not found at $ExecutablePath"
   exit 1
}
