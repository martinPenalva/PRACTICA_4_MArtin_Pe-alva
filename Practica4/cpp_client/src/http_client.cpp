#include "http_client.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <sstream>
#include <stdexcept>

using nlohmann::json;

static size_t writeToString(void* contents, size_t size, size_t nmemb, void* userp) {
  const size_t total = size * nmemb;
  auto* s = static_cast<std::string*>(userp);
  s->append(static_cast<char*>(contents), total);
  return total;
}

static json matrixToJson2D(const Matrix& m) {
  json out = json::array();
  for (const auto& row : m) {
    json jr = json::array();
    for (double v : row) jr.push_back(v);
    out.push_back(std::move(jr));
  }
  return out;
}

std::string postCaptureMatrix(const std::string& url, int captureId, const Matrix& matrix128) {
  const int height = static_cast<int>(matrix128.size());
  const int width = height > 0 ? static_cast<int>(matrix128[0].size()) : 0;

  if (height != 128 || width != 128) {
    throw std::runtime_error("postCaptureMatrix: expected 128x128 matrix");
  }

  json payload;
  payload["capture_id"] = captureId;
  payload["width"] = 128;
  payload["height"] = 128;
  payload["data"] = matrixToJson2D(matrix128);

  const std::string body = payload.dump();

  CURL* curl = curl_easy_init();
  if (!curl) throw std::runtime_error("curl_easy_init failed");

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode res = curl_easy_perform(curl);

  long httpCode = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    throw std::runtime_error(std::string("HTTP request failed: ") + curl_easy_strerror(res));
  }
  if (httpCode < 200 || httpCode >= 300) {
    std::ostringstream oss;
    oss << "Server returned HTTP " << httpCode << ": " << response;
    throw std::runtime_error(oss.str());
  }

  return response;
}

