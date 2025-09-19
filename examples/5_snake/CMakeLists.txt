cmake_minimum_required(VERSION 3.28)

project(5_snake)

set(GAME_ENGINE_SDK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SNAKE_SOURCES "src/*.cpp")
add_executable(5_snake ${SNAKE_SOURCES})

target_compile_features(5_snake PUBLIC cxx_std_20)

add_subdirectory(${GAME_ENGINE_ROOT_DIR} game_engine_sdk)

target_include_directories(5_snake
    PRIVATE
        ${GAME_ENGINE_SDK_SOURCE_DIR}/include
)

target_link_libraries(5_snake
    PRIVATE
    game_engine_sdk
)

# Output directory for binaries
set_target_properties(5_snake
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
