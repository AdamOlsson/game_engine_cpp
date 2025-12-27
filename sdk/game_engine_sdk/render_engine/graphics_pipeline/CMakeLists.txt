cmake_minimum_required(VERSION 3.28)

option(CMAKE_LOG_LEVEL_DEBUG "Configure using debug log level" OFF)

message(STATUS "Building graphics_pipeline module with the following options:")
message(STATUS "    Log level: ${CMAKE_LOG_LEVEL_DEBUG}")
message(STATUS "    Build type: ${CMAKE_BUILD_TYPE}")

project(graphics_pipeline CXX)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h")

foreach(source_file ${SOURCES})
    message(STATUS "Found source file: ${source_file}")
endforeach()

# Compile shaders
execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/src/quad/shaders/compile_shaders.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE result
)

execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry/shaders/compile_shaders.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE result
)

execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/src/text/shaders/compile_shaders.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE result
)

if(result)
    message(FATAL_ERROR "Failed to compile shaders with error: ${result}")
endif()

add_library(${PROJECT_NAME} 
    STATIC
        ${SOURCES}
)
add_library(render_engine::graphics_pipeline ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src

)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        sdk::logger
        sdk::util
        glm::glm
        render_engine::vulkan
        render_engine::font
        render_engine::triangulation
)
