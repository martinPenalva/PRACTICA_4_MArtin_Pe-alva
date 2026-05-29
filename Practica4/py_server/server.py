from __future__ import annotations

import os
from pathlib import Path
from typing import Any

import numpy as np
from flask import Flask, jsonify, request

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402


APP = Flask(__name__)
IMAGES_DIR = Path(__file__).resolve().parent / "images"


def _validate_payload(payload: dict[str, Any]) -> tuple[int, int, int, list[list[float]]]:
    if "capture_id" not in payload:
        raise ValueError("Missing field: capture_id")
    if "width" not in payload or "height" not in payload:
        raise ValueError("Missing field: width/height")
    if "data" not in payload:
        raise ValueError("Missing field: data")

    capture_id = int(payload["capture_id"])
    width = int(payload["width"])
    height = int(payload["height"])
    data = payload["data"]

    if width != 128 or height != 128:
        raise ValueError("Expected width=128 and height=128")
    if not isinstance(data, list) or len(data) != height:
        raise ValueError("data must be a 128x128 nested list")
    for row in data:
        if not isinstance(row, list) or len(row) != width:
            raise ValueError("data must be a 128x128 nested list")

    return capture_id, width, height, data


def _save_tactile_image(capture_id: int, matrix: np.ndarray) -> str:
    IMAGES_DIR.mkdir(parents=True, exist_ok=True)
    out_path = IMAGES_DIR / f"capture_{capture_id}.png"

    plt.figure(figsize=(6, 5), dpi=150)
    plt.imshow(matrix, cmap="inferno")
    plt.colorbar(label="Presion")
    plt.title("Mapa tactil")
    plt.tight_layout()
    plt.savefig(out_path, bbox_inches="tight")
    plt.close()

    return str(out_path)


@APP.post("/capture")
def capture() -> Any:
    try:
        payload = request.get_json(force=True, silent=False)
        if not isinstance(payload, dict):
            raise ValueError("JSON body must be an object")

        capture_id, _, _, data = _validate_payload(payload)
        matrix = np.array(data, dtype=np.float64)

        path = _save_tactile_image(capture_id, matrix)
        return jsonify({"ok": True, "saved": os.path.basename(path)})
    except Exception as e:
        return jsonify({"ok": False, "error": str(e)}), 400


@APP.get("/health")
def health() -> Any:
    return jsonify({"ok": True})


if __name__ == "__main__":
    APP.run(host="127.0.0.1", port=5000, debug=False)

