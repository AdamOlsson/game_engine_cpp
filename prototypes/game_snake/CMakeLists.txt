cmake_minimum_required(VERSION 3.28)

set(PROTOTYPE_NAME "game_snake")
project(${PROTOTYPE_NAME})

set(GAME_ENGINE_SDK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build the game engine sdk
add_subdirectory(${GAME_ENGINE_SDK_SOURCE_DIR} game_engine_sdk)

file(GLOB_RECURSE PROTOTYPE_SOURCES "src/*.cpp")
add_executable(${PROTOTYPE_NAME} ${PROTOTYPE_SOURCES})
target_compile_features(${PROTOTYPE_NAME} PUBLIC cxx_std_20)


target_include_directories(${PROTOTYPE_NAME}
    PRIVATE
        ${GAME_ENGINE_SDK_SOURCE_DIR}/include
)

target_link_libraries(${PROTOTYPE_NAME}
    PRIVATE
    game_engine_sdk
)

set_target_properties(${PROTOTYPE_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
