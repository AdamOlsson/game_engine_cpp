cmake_minimum_required(VERSION 3.28)

option(BUILD_LOCAL_SDK "Build the local game engine SDK" OFF)

set(GAME_ENGINE_SDK_VERSION "0.0.1")

set(PROTOTYPE_NAME "game_snake")
project(${PROTOTYPE_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GAME_ENGINE_SDK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(GAME_ENGINE_SDK_BIN_DIR "${GAME_ENGINE_SDK_SOURCE_DIR}/releases/v${GAME_ENGINE_SDK_VERSION}/dist")

####### Depenceies
include(${GAME_ENGINE_SDK_SOURCE_DIR}/cmake/vulkan.cmake)
include(${GAME_ENGINE_SDK_SOURCE_DIR}/cmake/glfw.cmake)

if(BUILD_LOCAL_SDK)
    message(STATUS "Preparing local game engine SDK")
    add_subdirectory(${GAME_ENGINE_SDK_SOURCE_DIR} GameEngineSDK::Engine)
else()
    message(STATUS "Using prebuilt game engine SDK")
    add_library(GameEngineSDK::Engine STATIC IMPORTED)
    set_target_properties(GameEngineSDK::Engine PROPERTIES
        IMPORTED_LOCATION ${GAME_ENGINE_SDK_BIN_DIR}/libgame_engine_sdk.a
    )
    target_include_directories(GameEngineSDK::Engine INTERFACE
        ${GAME_ENGINE_SDK_BIN_DIR}/include
    )
endif()

file(GLOB_RECURSE PROTOTYPE_SOURCES "src/*.cpp")
add_executable(${PROTOTYPE_NAME} ${PROTOTYPE_SOURCES})
target_compile_features(${PROTOTYPE_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROTOTYPE_NAME}
    PRIVATE
        ${GAME_ENGINE_SDK_SOURCE_DIR}/include
        ${VULKAN_SDK_PATH}/include
        ${glfw_SOURCE_DIR}/include
)

target_link_libraries(${PROTOTYPE_NAME}
    PRIVATE
        GameEngineSDK::Engine 
        ${VULKAN_LIBRARY}
        glfw
)

set_target_properties(${PROTOTYPE_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
