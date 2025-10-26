cmake_minimum_required(VERSION 3.28)

# Global options
option(GAME_ENGINE_SDK_BUILD_TEST "Build the unit tests" OFF)
option(CMAKE_LOG_LEVEL_DEBUG "Configure using debug log level" OFF)

message(STATUS "Configuring game_engine_sdk with the following options:")
message(STATUS "    Log level: ${CMAKE_LOG_LEVEL_DEBUG}")
message(STATUS "    Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "    Build tests: ${GAME_ENGINE_SDK_BUILD_TEST}")

project(game_engine_sdk CXX)

set(SDK_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. CACHE INTERNAL "SDK root include directory")

option(GAME_ENGINE_SDK_BUILD_RENDER_ENGINE "Build the render engine" ON)

message(STATUS "")
message(STATUS "Configuring game_engine_sdk modules:")

# Add render_engine
if(GAME_ENGINE_SDK_BUILD_RENDER_ENGINE)
    message(STATUS "  - Adding render_engine")
    add_subdirectory(render_engine)
else()
    message(STATUS "  - Skipping render_engine")
endif()


message(STATUS "")
