cmake_minimum_required(VERSION 3.16) 

option(CMAKE_LOG_LEVEL_DEBUG "Configure using debug log level" OFF)
message(STATUS "Building interface module with the following options:")
message(STATUS "    Log level: ${CMAKE_LOG_LEVEL_DEBUG}")
message(STATUS "    Build type: ${CMAKE_BUILD_TYPE}")

project(interface CXX)

add_library(${PROJECT_NAME} INTERFACE) 

add_library(sdk::interface ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME}
    INTERFACE 
        # Using a relative include dir is often safer than absolute, 
        # but keeping your path logic:
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>") 

target_link_libraries(${PROJECT_NAME}
    INTERFACE 
        sdk::math
)
