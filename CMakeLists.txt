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

install(TARGETS Logger DESTINATION bin)

configure_file(
    kernel-telemetry.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/kernel-telemetry.service
    @ONLY
)

install(PROGRAMS install_setup.sh
DESTINATION /tmp
RENAME run_telemetry_setup.sh
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel-telemetry.service
    DESTINATION /etc/systemd/system
)

install(CODE "
execute_process(
COMMAND /bin/bash /tmp/run_telemetry_setup.sh
WORKING_DIRECTORY /
OUTPUT_VARIABLE SCRIPT_ERROR
RESULT_VARIABLE SCRIPT_RESULT
)
if(NOT SCRIPT_RESULT EQUAL 0)
    message(FATAL_ERROR \"Setup script failed with error: \n${SCRIPT_ERROR}\")
    endif()
")

install(CODE "
execute_process(
COMMAND systemctl daemon-reload
RESULT_VARIABLE RELOAD_RESULT
)
if(NOT RELOAD_RESULT EQUAL 0)
    message(FATAL_ERROR \"systemctl daemon-reload failed.\")
endif()

execute_process(
COMMAND systemctl enable kernel-telemetry.service
RESULT_VARIABLE ENABLE_RESULT
)
if(NOT ENABLE_RESULT EQUAL 0)
    message(FATAL_ERROR \"systemctl enable failed.\")
endif()

execute_process(
COMMAND systemctl start kernel-telemetry.service
RESULT_VARIABLE START_RESULT
)
if(NOT START_RESULT EQUAL 0)
    message(FATAL_ERROR \"systemctl start failed.\")
endif()

message(STATUS \"Service kernel-telemetry.service enabled and started successfully.\")
")

install(CODE "
execute_process(
COMMAND rm -f /tmp/run_telemetry_setup.sh
WORKING_DIRECTORY / 
)
")