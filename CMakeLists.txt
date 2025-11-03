cmake_minimum_required(VERSION 3.28)

option(GAME_ENGINE_SDK_BUILD_TEST "Build the unit tests" OFF)
option(GAME_ENGINE_SDK_BUILD_EXAMPLES "Build the examples" OFF)
option(CMAKE_LOG_LEVEL_DEBUG "Configure using debug log level" OFF)

message(STATUS "Building game engine SDK with the following options:")
message(STATUS "    Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "    Build tests: ${GAME_ENGINE_SDK_BUILD_TEST}")
message(STATUS "    Build examples: ${GAME_ENGINE_SDK_BUILD_EXAMPLES}")

# include(cmake/llvm.cmake)

message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

project(game_engine_sdk CXX)

include(cmake/cmake-global-settings.cmake)
include(cmake/vulkan.cmake)
include(cmake/glm.cmake)
include(cmake/glfw.cmake)

file(GLOB_RECURSE GAME_ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GAME_ENGINE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

if(CMAKE_LOG_LEVEL_DEBUG)
foreach(header ${GAME_ENGINE_HEADERS})
    message(STATUS "Found header: ${header}")
endforeach()

foreach(source_file ${GAME_ENGINE_SOURCES})
    message(STATUS "Found source file: ${source_file}")
endforeach()

foreach(source_file ${GAME_ENGINE_MODULES})
    message(STATUS "Found module file: ${source_file}")
endforeach()
endif()

if(GAME_ENGINE_SDK_BUILD_TEST)
    include(cmake/gtest.cmake)
    enable_testing()
endif()

set(GAME_ENGINE_SDK_BUILD_RENDER_ENGINE ON CACHE BOOL "" FORCE)
set(RENDER_ENGINE_BUILD_IMAGE ON CACHE BOOL "" FORCE)
set(RENDER_ENGINE_BUILD_CAMERA ON CACHE BOOL "" FORCE)
set(RENDER_ENGINE_BUILD_TILING ON CACHE BOOL "" FORCE)
set(RENDER_ENGINE_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(RENDER_ENGINE_BUILD_VULKAN ON CACHE BOOL "" FORCE)
add_subdirectory(sdk/game_engine_sdk)

add_library(${PROJECT_NAME}
    STATIC 
        ${GAME_ENGINE_SOURCES} 
        ${GAME_ENGINE_HEADERS} 
)

add_library(GameEngineSDK::Engine ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${VULKAN_SDK_PATH}/include
        ${glfw_SOURCE_DIR}/include
)


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${VULKAN_LIBRARY}
        glm
        glfw
        render_engine::image
        render_engine::camera
        render_engine::tiling
        render_engine::window
        render_engine::vulkan
        sdk::logger
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(GAME_ENGINE_SDK_BUILD_TEST)
    include(cmake/tests.cmake)
endif()

if(GAME_ENGINE_SDK_BUILD_EXAMPLES)
    message(STATUS "Configuring examples...")
    add_subdirectory(examples/1_spatial_subdivision)
    add_subdirectory(examples/2_shape_rendering)
    add_subdirectory(examples/3_tile_showcase)
    add_subdirectory(examples/4_user_interface)
    add_subdirectory(examples/5_map_generation)
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

if(CMAKE_BUILD_TYPE STREQUAL "release")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_BINARY_DIR}/dist/include
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_BINARY_DIR}/dist/
        COMMENT "Copying headers and library to distribution directory"
    )
endif()
