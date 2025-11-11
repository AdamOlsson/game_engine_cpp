message(STATUS "Building tests...")

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
set(TEST_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_utils.h")
add_executable(unit_tests 
    ${TEST_SOURCES}
    ${TEST_HEADERS}
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(unit_tests
    PRIVATE
    ${PROJECT_NAME}
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
set_target_properties(unit_tests
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

message(STATUS "Building tests... DONE!")

