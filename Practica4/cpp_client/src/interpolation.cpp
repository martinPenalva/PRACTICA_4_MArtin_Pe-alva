#include "interpolation.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

static double atClamped(const Matrix& m, int r, int c) {
  const int rows = static_cast<int>(m.size());
  const int cols = rows > 0 ? static_cast<int>(m[0].size()) : 0;
  r = std::clamp(r, 0, rows - 1);
  c = std::clamp(c, 0, cols - 1);
  return m[r][c];
}

Matrix bilinearResize(const Matrix& src, int outRows, int outCols) {
  const int inRows = static_cast<int>(src.size());
  const int inCols = inRows > 0 ? static_cast<int>(src[0].size()) : 0;

  if (inRows <= 0 || inCols <= 0) {
    throw std::runtime_error("bilinearResize: source matrix is empty");
  }
  for (const auto& row : src) {
    if (static_cast<int>(row.size()) != inCols) {
      throw std::runtime_error("bilinearResize: source matrix is not rectangular");
    }
  }
  if (outRows <= 0 || outCols <= 0) {
    throw std::runtime_error("bilinearResize: invalid output size");
  }

  Matrix out(outRows, std::vector<double>(outCols, 0.0));

  // Map output pixel centers to input coordinate space [0..in-1]
  const double rowScale = (outRows == 1) ? 0.0 : static_cast<double>(inRows - 1) / static_cast<double>(outRows - 1);
  const double colScale = (outCols == 1) ? 0.0 : static_cast<double>(inCols - 1) / static_cast<double>(outCols - 1);

  for (int r = 0; r < outRows; r++) {
    const double srcR = rowScale * r;
    const int r0 = static_cast<int>(std::floor(srcR));
    const int r1 = r0 + 1;
    const double dr = srcR - r0;

    for (int c = 0; c < outCols; c++) {
      const double srcC = colScale * c;
      const int c0 = static_cast<int>(std::floor(srcC));
      const int c1 = c0 + 1;
      const double dc = srcC - c0;

      const double v00 = atClamped(src, r0, c0);
      const double v01 = atClamped(src, r0, c1);
      const double v10 = atClamped(src, r1, c0);
      const double v11 = atClamped(src, r1, c1);

      const double v0 = v00 * (1.0 - dc) + v01 * dc;
      const double v1 = v10 * (1.0 - dc) + v11 * dc;
      out[r][c] = v0 * (1.0 - dr) + v1 * dr;
    }
  }

  return out;
}

