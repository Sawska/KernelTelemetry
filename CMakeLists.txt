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

# install(PROGRAMS install_setup.sh
# DESTINATION /tmp
# RENAME run_telemetry_setup.sh
# )


# install(CODE "
# execute_process(
# COMMAND ${CMAKE_INSTALL_PREFIX}/tmp/run_telemetry_setup.sh
# WORKING DIRECTORY /

# OUTPUT_VARIABLE SCRIPT_ERROR
# RESUTL_VARIABLE SCRIPT_RESULT
# )
# if(NOT SCRIPT_RESULT EQUAL 0)
#     message(FATAL_ERROR \"Setup script failed with error: \n${SCRIPT_ERROR}\")
#     endif()
# ")

# install(CODE "
# execute_process(
# COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/tmp/run_telemetry_setup.sh
# WORKING_DIRECTORY / 
# )
# ")