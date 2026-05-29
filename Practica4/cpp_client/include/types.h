#pragma once

#include <vector>

using Matrix = std::vector<std::vector<double>>;

struct Capture {
  int id;
  Matrix matrix16;
};

