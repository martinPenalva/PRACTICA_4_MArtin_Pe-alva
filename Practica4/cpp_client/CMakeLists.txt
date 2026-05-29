cmake_minimum_required(VERSION 3.20)
project(tactile_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# nlohmann/json (header-only)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(CURL REQUIRED)

add_executable(tactile_client
  src/main.cpp
  src/tactile_json.cpp
  src/interpolation.cpp
  src/http_client.cpp
)

target_include_directories(tactile_client PRIVATE include)

target_link_libraries(tactile_client PRIVATE nlohmann_json::nlohmann_json CURL::libcurl)

