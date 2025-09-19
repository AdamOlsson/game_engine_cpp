cmake_minimum_required(VERSION 3.28)

set(EXAMPLE_NAME "2_shape_rendering")

message(STATUS "Configuring example ${EXAMPLE_NAME}...")
project(${EXAMPLE_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable(${EXAMPLE_NAME} ${SOURCE_FILES})
target_compile_features(${EXAMPLE_NAME} PUBLIC cxx_std_20)

target_include_directories(${EXAMPLE_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${EXAMPLE_NAME}
    PRIVATE
        game_engine_sdk
)

set_target_properties(${EXAMPLE_NAME}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


message(STATUS "Configuring example ${EXAMPLE_NAME}... DONE!")
