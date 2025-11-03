cmake_minimum_required(VERSION 3.28)

option(CMAKE_LOG_LEVEL_DEBUG "Configure using debug log level" OFF)

message(STATUS "Building util module with the following options:")
message(STATUS "    Log level: ${CMAKE_LOG_LEVEL_DEBUG}")
message(STATUS "    Build type: ${CMAKE_BUILD_TYPE}")

project(util CXX)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h")

add_library(${PROJECT_NAME} 
    STATIC
        ${SOURCES}
)
add_library(sdk::util ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)

# target_link_libraries(${PROJECT_NAME}
#     PRIVATE 
#         # sdk::logger
#         # glm::glm
# )
