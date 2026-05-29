#include "tactile_json.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using nlohmann::json;

bool isMatrixSize(const Matrix& m, int rows, int cols) {
  if (static_cast<int>(m.size()) != rows) return false;
  for (const auto& row : m) {
    if (static_cast<int>(row.size()) != cols) return false;
  }
  return true;
}

static Matrix parseMatrix16(const json& jMatrix, int captureId) {
  if (!jMatrix.is_array() || jMatrix.size() != 16) {
    throw std::runtime_error("Capture " + std::to_string(captureId) + ": matrix must have 16 rows");
  }

  Matrix m(16, std::vector<double>(16, 0.0));
  for (int r = 0; r < 16; r++) {
    const auto& row = jMatrix.at(r);
    if (!row.is_array() || row.size() != 16) {
      throw std::runtime_error("Capture " + std::to_string(captureId) + ": matrix row must have 16 cols");
    }
    for (int c = 0; c < 16; c++) {
      m[r][c] = row.at(c).get<double>();
    }
  }
  return m;
}

std::vector<Capture> readCapturesFromJsonFile(const std::string& jsonPath) {
  std::ifstream in(jsonPath);
  if (!in) {
    throw std::runtime_error("Cannot open JSON file: " + jsonPath);
  }

  json root;
  in >> root;

  if (!root.contains("captures") || !root["captures"].is_array()) {
    throw std::runtime_error("JSON must contain array field: captures");
  }

  std::vector<Capture> captures;
  for (const auto& cap : root["captures"]) {
    if (!cap.contains("id") || !cap.contains("matrix")) {
      throw std::runtime_error("Each capture must contain fields: id, matrix");
    }
    int id = cap["id"].get<int>();
    Matrix m16 = parseMatrix16(cap["matrix"], id);
    if (!isMatrixSize(m16, 16, 16)) {
      throw std::runtime_error("Capture " + std::to_string(id) + ": matrix is not 16x16");
    }
    captures.push_back(Capture{id, std::move(m16)});
  }

  return captures;
}

