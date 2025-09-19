cmake_minimum_required(VERSION 3.28)

option(GAME_ENGINE_SDK_BUILD_TEST "Build the unit tests" OFF)
option(GAME_ENGINE_SDK_BUILD_EXAMPLES "Build the examples" OFF)

message(STATUS "Building game engine SDK with the following options:")
message(STATUS "    Build tests: ${GAME_ENGINE_SDK_BUILD_TEST}")
message(STATUS "    Build examples: ${GAME_ENGINE_SDK_BUILD_EXAMPLES}")

# include(cmake/llvm.cmake)

message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

project(game_engine_sdk CXX)

include(cmake/cmake-global-settings.cmake)
include(cmake/vulkan.cmake)
include(cmake/glfw.cmake)

file(GLOB_RECURSE GAME_ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GAME_ENGINE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

foreach(header ${GAME_ENGINE_HEADERS})
    message(STATUS "Found header: ${header}")
endforeach()

foreach(source_file ${GAME_ENGINE_SOURCES})
    message(STATUS "Found source file: ${source_file}")
endforeach()

foreach(source_file ${GAME_ENGINE_MODULES})
    message(STATUS "Found module file: ${source_file}")
endforeach()

add_library(game_engine_sdk
    STATIC 
        ${GAME_ENGINE_SOURCES} 
        ${GAME_ENGINE_HEADERS} 
)
target_compile_features(game_engine_sdk PUBLIC cxx_std_20)

target_include_directories(game_engine_sdk
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${VULKAN_SDK_PATH}/include
        ${glfw_SOURCE_DIR}/include
)

target_link_libraries(game_engine_sdk
    PUBLIC
        ${VULKAN_LIBRARY}
        glfw
)


set_target_properties(game_engine_sdk
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(GAME_ENGINE_SDK_BUILD_TEST)
    message(STATUS "Building tests...")
    # Tests configuration
    include(cmake/gtest.cmake)
    enable_testing()
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
        game_engine_sdk
            GTest::gtest_main
    )
    
    include(GoogleTest)
    gtest_discover_tests(unit_tests)
    set_target_properties(unit_tests
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    message(STATUS "Building tests... DONE!")
endif()

if(GAME_ENGINE_SDK_BUILD_EXAMPLES)
    message(STATUS "Configuring examples...")
    add_subdirectory(examples/1_spatial_subdivision)
    add_subdirectory(examples/2_shape_rendering)
    add_subdirectory(examples/4_user_interface)
    message(STATUS "Configuring examples... DONE!")
endif()


# Clean Target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
)

# Create a symbolic link to compile_commands.json in project root
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()
