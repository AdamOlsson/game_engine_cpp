cmake_minimum_required(VERSION 3.28)

project(5_snake)

set(GAME_ENGINE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../build/lib")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


file(GLOB_RECURSE SNAKE_SOURCES "src/*.cpp")
add_executable(5_snake ${SNAKE_SOURCES})

target_link_libraries(5_snake
    PRIVATE
        "${GAME_ENGINE_SDK_DIR}/libgame_engine.a"  # Direct path to .a file
)

target_link_libraries(5_snake
    PRIVATE
        game_engine
)
# Output directory for binaries
set_target_properties(5_snake
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
