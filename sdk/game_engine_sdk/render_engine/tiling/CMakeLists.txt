cmake_minimum_required(VERSION 3.28)

option(GAME_ENGINE_SDK_BUILD_TEST "Build the unit tests" OFF)
option(CMAKE_LOG_LEVEL_DEBUG "Confgure using debug log level" OFF)

message(STATUS "Building tiling module with the following options:")
message(STATUS "    Log level: ${CMAKE_LOG_LEVEL_DEBUG}")
message(STATUS "    Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "    Build tests: ${GAME_ENGINE_SDK_BUILD_TEST}")

project(tiling CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(render_engine::tiling ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME} 
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

if(GAME_ENGINE_SDK_BUILD_TEST)
    add_subdirectory(test)
endif()
