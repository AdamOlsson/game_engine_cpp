
message(STATUS "Building tests for ${PROJECT_NAME}...")

if(NOT GTest_FOUND)
    message(FATAL_ERROR "Could not find GTest!")
endif()

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(CMAKE_LOG_LEVEL_DEBUG)
    message(STATUS "Found test source files:")
    foreach(source_file ${TEST_SOURCES})
        message(STATUS "    - ${source_file}")
    endforeach()
endif()

set(TEST_EXEC_NAME "font_tests")

add_executable(${TEST_EXEC_NAME}
    ${TEST_SOURCES}
)

target_include_directories(${TEST_EXEC_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${TEST_EXEC_NAME}
    PRIVATE
        render_engine::font
        sdk::util
        GTest::gtest_main
)

target_compile_features(${TEST_EXEC_NAME} PRIVATE cxx_std_20)

set_target_properties(${TEST_EXEC_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

include(GoogleTest)
gtest_discover_tests(${TEST_EXEC_NAME})


message(STATUS "Building tests for ${PROJECT_NAME}... DONE!")

