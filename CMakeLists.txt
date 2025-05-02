cmake_minimum_required(VERSION 3.15)

message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.4.sdk")
project(game_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VULKAN_SDK_PATH "/Users/adamolsson/VulkanSDK/1.3.296.0/macOS")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/opt/homebrew")

set(Vulkan_INCLUDE_DIR "${VULKAN_SDK_PATH}/include")
set(Vulkan_LIBRARY "${VULKAN_SDK_PATH}/lib/libvulkan.1.3.296.dylib")

find_package(glfw3 REQUIRED)

cmake_policy(SET CMP0135 NEW) 
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE GAME_ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GAME_ENGINE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

foreach(header ${GAME_ENGINE_HEADERS})
    message(STATUS "Found header: ${header}")
endforeach()

foreach(source_file ${GAME_ENGINE_SOURCES})
    message(STATUS "Found source file: ${source_file}")
endforeach()


add_library(game_engine 
    STATIC 
        ${GAME_ENGINE_SOURCES} 
        ${GAME_ENGINE_HEADERS} 
)

target_include_directories(game_engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(game_engine
    PUBLIC
        ${Vulkan_LIBRARY}
        glfw
)

set_target_properties(game_engine
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

file(GLOB_RECURSE DEV_SOURCES "examples/0_collision_detection/*.cpp")
add_executable(0_collision_detection ${DEV_SOURCES})
target_include_directories(0_collision_detection
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(0_collision_detection
    PRIVATE
        game_engine
)

# Output directory for binaries
set_target_properties(0_collision_detection
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(GLOB_RECURSE DEV_SOURCES "examples/1_spatial_subdivision/*.cpp")
add_executable(1_spatial_subdivision ${DEV_SOURCES})
target_include_directories(1_spatial_subdivision
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(1_spatial_subdivision
    PRIVATE
        game_engine
)
# Output directory for binaries
set_target_properties(1_spatial_subdivision
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(GLOB_RECURSE DEV_SOURCES "examples/2_shape_rendering/*.cpp")
add_executable(2_shape_rendering ${DEV_SOURCES})
target_include_directories(2_shape_rendering
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(2_shape_rendering
    PRIVATE
        game_engine
)
# Output directory for binaries
set_target_properties(2_shape_rendering
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(GLOB_RECURSE DEV_SOURCES "examples/3_collision_scenarios/*.cpp")
add_executable(3_collision_scenarios ${DEV_SOURCES})
target_include_directories(3_collision_scenarios
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(3_collision_scenarios
    PRIVATE
        game_engine
)
# Output directory for binaries
set_target_properties(3_collision_scenarios
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Tests configuration
enable_testing()
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(unit_tests ${TEST_SOURCES})
target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(unit_tests
    PRIVATE
        game_engine
        GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(unit_tests)

# Output directory for test binary
set_target_properties(unit_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

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
