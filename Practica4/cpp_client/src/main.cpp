#include "http_client.h"
#include "interpolation.h"
#include "tactile_json.h"

#include <curl/curl.h>

#include <iostream>
#include <stdexcept>
#include <string>

static void printUsage(const char* exe) {
  std::cerr << "Usage:\n"
            << "  " << exe
            << " <path_to_tactile_captures_50.json> <server_url>\n"
            << "Example:\n"
            << "  " << exe
            << " ..\\tactile_captures_50.json http://127.0.0.1:5000/capture\n";
}

int main(int argc, char** argv) {
  try {
    std::string jsonPath = "..\\tactile_captures_50.json";
    std::string url = "http://127.0.0.1:5000/capture";

    if (argc == 3) {
      jsonPath = argv[1];
      url = argv[2];
    } else if (argc != 1) {
      printUsage(argv[0]);
      return 2;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    const auto captures = readCapturesFromJsonFile(jsonPath);
    if (captures.empty()) {
      throw std::runtime_error("No captures found in JSON");
    }

    for (const auto& cap : captures) {
      if (!isMatrixSize(cap.matrix16, 16, 16)) {
        throw std::runtime_error("Capture " + std::to_string(cap.id) + ": invalid 16x16 matrix");
      }

      Matrix m128 = bilinearResize(cap.matrix16, 128, 128);
      const std::string resp = postCaptureMatrix(url, cap.id, m128);
      std::cout << "Capture " << cap.id << " -> OK (" << resp << ")\n";
    }

    curl_global_cleanup();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}

