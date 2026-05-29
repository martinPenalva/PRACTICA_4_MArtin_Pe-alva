param(
  [string]$JsonPath = ".\tactile_captures_50.json",
  [string]$Url = "http://127.0.0.1:5000/capture",
  [ValidateSet("Release", "Debug")]
  [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"

function Find-VcpkgCMake {
  $root = Join-Path $PSScriptRoot "vcpkg"

  # Typical vcpkg tool path (downloaded automatically)
  $candidates = @()
  $candidates += Get-ChildItem -Path (Join-Path $root "downloads\tools") -Recurse -Filter "cmake.exe" -ErrorAction SilentlyContinue
  $candidates += Get-ChildItem -Path (Join-Path $root "downloads\tools") -Recurse -Filter "cmake" -ErrorAction SilentlyContinue

  foreach ($c in $candidates) {
    if ($c.FullName -match "\\bin\\cmake\.exe$") { return $c.FullName }
  }

  # Fallback: hope cmake is in PATH
  $cmd = Get-Command cmake -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }

  return $null
}

function Ensure-Vcpkg {
  $vcpkgExe = Join-Path $PSScriptRoot "vcpkg\vcpkg.exe"
  if (-not (Test-Path $vcpkgExe)) {
    throw "No existe $vcpkgExe. Ejecuta primero: git clone https://github.com/microsoft/vcpkg .\vcpkg ; .\vcpkg\bootstrap-vcpkg.bat"
  }
  return $vcpkgExe
}

$vcpkgExe = Ensure-Vcpkg

Write-Host "Instalando dependencia curl (vcpkg)..." -ForegroundColor Cyan
& $vcpkgExe install curl:x64-windows | Out-Host

$cmake = Find-VcpkgCMake
if (-not $cmake) {
  throw "No se encontró CMake (cmake.exe)."
}

$clientDir = Join-Path $PSScriptRoot "cpp_client"
$buildDir = Join-Path $clientDir "build_vcpkg"
$toolchain = Join-Path $PSScriptRoot "vcpkg\scripts\buildsystems\vcpkg.cmake"

if (Test-Path (Join-Path $clientDir "build\\CMakeCache.txt")) {
  $cache = Get-Content -Path (Join-Path $clientDir "build\\CMakeCache.txt") -Raw -ErrorAction SilentlyContinue
  if ($cache -and ($cache -match 'CMAKE_TOOLCHAIN_FILE:UNINITIALIZED=\$toolchain')) {
    Write-Host "Aviso: existe un build anterior con toolchain incorrecto en cpp_client\\build\\. Se ignorará y se usará cpp_client\\build_vcpkg\\." -ForegroundColor Yellow
  }
}

Write-Host "Configurando CMake..." -ForegroundColor Cyan
& $cmake -S "$clientDir" -B "$buildDir" "-DCMAKE_TOOLCHAIN_FILE=$toolchain" "-DVCPKG_TARGET_TRIPLET=x64-windows" | Out-Host

Write-Host "Compilando ($Config)..." -ForegroundColor Cyan
& $cmake --build "$buildDir" --config $Config | Out-Host

$exe = Join-Path $buildDir "$Config\tactile_client.exe"
if (-not (Test-Path $exe)) {
  # single-config generator fallback
  $exe = Join-Path $buildDir "tactile_client.exe"
}
if (-not (Test-Path $exe)) {
  throw "No se encontró el ejecutable del cliente tras compilar."
}

Write-Host "Ejecutando cliente -> $Url" -ForegroundColor Cyan
& $exe $JsonPath $Url

