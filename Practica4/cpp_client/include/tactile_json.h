#pragma once

#include <string>
#include <vector>

#include "types.h"

std::vector<Capture> readCapturesFromJsonFile(const std::string& jsonPath);

bool isMatrixSize(const Matrix& m, int rows, int cols);

