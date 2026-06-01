## Ejecución (método único) — prueba end-to-end

Este método usa **vcpkg** para instalar `libcurl` y **winget** para instalar `cmake` (para evitar el error “cmake no se reconoce”).

### 0) Requisitos

- Python 3
- Visual Studio (MSVC) o Build Tools
- Git
- CMake instalado (paso 2)

### 1) Lanzar el servidor (Python)

```powershell
cd "c:\Users\Martin\Documents\martin\2026\UBU\Sistemas operativos\PRACTICAS\PRACTICA_4\Practica4\py_server"
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements.txt
python server.py
```

Déjalo corriendo (escucha en `http://127.0.0.1:5000`).

### 2) Compilar y ejecutar el cliente (C++)

En otra terminal:

```powershell
cd "c:\Users\Martin\Documents\martin\2026\UBU\Sistemas operativos\PRACTICAS\PRACTICA_4\Practica4"

# instalar CMake (una sola vez). Luego cierra y abre la terminal.
winget install --id Kitware.CMake -e

# (solo la primera vez)
git clone https://github.com/microsoft/vcpkg .\vcpkg
.\vcpkg\bootstrap-vcpkg.bat

.\vcpkg\vcpkg.exe install curl:x64-windows

cd .\cpp_client
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="..\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build build --config Release

.\build\Release\tactile_client.exe "..\tactile_captures_50.json" "http://127.0.0.1:5000/capture"
```

### 3) Comprobación

Si todo va bien:

- El cliente mostrará `Capture <id> -> OK ...` para las 50 capturas del archivo `tactile_captures_50.json`.
- Se generarán las imágenes en `py_server\images\`:
  - `capture_0.png`
  - ...
  - `capture_49.png`

