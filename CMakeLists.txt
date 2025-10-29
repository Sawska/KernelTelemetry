cmake_minimum_required(VERSION 3.14)

project(KernelTemetry VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(nlohmann_json)





add_executable(Logger main.cpp TelemetryCollector.cpp FileLogger.cpp)

target_include_directories(Logger PRIVATE ${nlohmann_json_SOURCE_DIR}/include)

