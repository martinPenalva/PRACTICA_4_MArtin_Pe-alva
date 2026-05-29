## Práctica 4 — Procesamiento e Interpolación de Imágenes Táctiles

Este proyecto implementa el flujo completo pedido en el enunciado:

- Cliente **C++**: lee `tactile_captures_50.json`, valida matrices 16×16, interpola **bilineal manual** a 128×128 y envía cada captura por **HTTP POST**.
- Servidor **Python (Flask)**: recibe las matrices, reconstruye el array y genera imágenes `capture_<id>.png` con Matplotlib.

### Estructura

- `cpp_client/`: cliente C++ (CMake)
- `py_server/`: servidor Flask + dependencias
- `tactile_captures_50.json`: dataset de 50 capturas (entrada)

---

## Servidor Python

### 1) Crear entorno e instalar dependencias

```bash
cd py_server
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements.txt
```

### 2) Ejecutar servidor

```bash
python server.py
```

El servidor escucha en `http://127.0.0.1:5000`.

Las imágenes se guardan en `py_server/images/` como:

- `capture_0.png`
- `capture_1.png`
- ...

---

## Cliente C++

### Requisitos

- CMake 3.20+
- Compilador C++17 (MSVC o MinGW)
- `libcurl` (headers + librería)

> Nota: `nlohmann/json` se descarga automáticamente desde CMake (FetchContent).

### Compilar (ejemplo)

```bash
cd cpp_client
cmake -S . -B build
cmake --build build --config Release
```

### Ejecutar

Desde `cpp_client/`:

```bash
.\build\Release\tactile_client.exe ..\tactile_captures_50.json http://127.0.0.1:5000/capture
```

Si compilas con MinGW o sin configuración multi-config:

```bash
.\build\tactile_client.exe ..\tactile_captures_50.json http://127.0.0.1:5000/capture
```

---

## Formato HTTP enviado (cliente → servidor)

```json
{
  "capture_id": 0,
  "width": 128,
  "height": 128,
  "data": [[...]]
}
```

