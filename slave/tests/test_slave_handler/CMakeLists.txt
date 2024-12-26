cmake_minimum_required(VERSION 3.11)
project(TestMasterHandler)

# Enable Testing
enable_testing()

# Compiler Flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-ggdb3 -O0 -pthread")

# Define projCOVERAGE_TEST
add_compile_definitions(projCOVERAGE_TEST=0)

# Include FetchContent module explicitly
include(FetchContent)

# Set FreeRTOS Path
set(FREERTOS_PATH /home/yancho/FreeRTOSv202212.01)

set(PROJECT_PATH /home/yancho/Projects/EnduroSat/state_synchronization)

# Include Directories
include_directories(
    ${PROJECT_PATH}/tests/include
    ${PROJECT_PATH}/slave/include
    ${PROJECT_PATH}/types
    ${PROJECT_PATH}/logger/include
    ${PROJECT_PATH}/config
    ${PROJECT_PATH}
    ${FREERTOS_PATH}/FreeRTOS/include
    ${FREERTOS_PATH}/FreeRTOS/Source/include
    ${FREERTOS_PATH}/FreeRTOS/Source/portable/ThirdParty/GCC/Posix
)

# Add GoogleTest and GoogleMock
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# Link GoogleTest and GoogleMock
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

# Enable UNIT_TEST during testing
add_compile_definitions(UNIT_TEST=1)

# Source Files
set(SOURCES
    ${PROJECT_PATH}/slave/src/slave_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_slave_handler.cpp
)

# Define the Test Executable
add_executable(test_master_handler ${SOURCES})

# Link Libraries
target_link_libraries(
    test_master_handler
    gtest
    gmock
    pthread
)

# Custom Target to Display LastTest.log After Tests
add_custom_target(show_test_log
    COMMAND ${CMAKE_COMMAND} -E cat ${CMAKE_BINARY_DIR}/Testing/Temporary/LastTest.log
    COMMENT "Displaying LastTest.log after test execution"
)

# Custom Target to Run Tests and Show Logs if Tests Fail
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${CMAKE_COMMAND} --build . --target show_test_log
    COMMENT "Running tests and displaying LastTest.log if failures occur"
)

# Add the Test to CTest
add_test(
    NAME TestMasterHandler
    COMMAND test_master_handler
)
