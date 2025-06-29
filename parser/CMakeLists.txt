cmake_minimum_required(VERSION 3.10)
project(log_parser CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  json_external
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json_external)

add_executable(log_parser src/main.cpp)
target_link_libraries(log_parser nlohmann_json::nlohmann_json)
