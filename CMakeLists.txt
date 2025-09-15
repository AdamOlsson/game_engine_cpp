cmake_minimum_required(VERSION 3.28)

# include(cmake/llvm.cmake)

message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

project(game_engine CXX)

include(cmake/cmake-global-settings.cmake)
include(cmake/vulkan.cmake)
include(cmake/glfw.cmake)
include(cmake/gtest.cmake)

file(GLOB_RECURSE GAME_ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GAME_ENGINE_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

foreach(header ${GAME_ENGINE_HEADERS})
    message(STATUS "Found header: ${header}")
endforeach()

foreach(source_file ${GAME_ENGINE_SOURCES})
    message(STATUS "Found source file: ${source_file}")
endforeach()

foreach(source_file ${GAME_ENGINE_MODULES})
    message(STATUS "Found module file: ${source_file}")
endforeach()


add_library(game_engine 
    STATIC 
        ${GAME_ENGINE_SOURCES} 
        ${GAME_ENGINE_HEADERS} 
)

target_include_directories(game_engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(game_engine
    PUBLIC
        ${Vulkan_LIBRARY}
        glfw
)

target_compile_features(game_engine PUBLIC cxx_std_20)

set_target_properties(game_engine
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)


# Tests configuration
enable_testing()
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
set(TEST_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_utils.h")
add_executable(unit_tests 
    ${TEST_SOURCES}
    ${TEST_HEADERS}
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(unit_tests
    PRIVATE
        game_engine
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
set_target_properties(unit_tests
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# file(GLOB_RECURSE DEV_SOURCES "examples/1_spatial_subdivision/*.cpp")
# add_executable(1_spatial_subdivision ${DEV_SOURCES})
# target_include_directories(1_spatial_subdivision
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
#         ${CMAKE_CURRENT_SOURCE_DIR}/src
# )
# target_link_libraries(1_spatial_subdivision
#     PRIVATE
#         game_engine
# )
# # Output directory for binaries
# set_target_properties(1_spatial_subdivision
#     PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#         CXX_SCAN_FOR_MODULES ON 
# )

# file(GLOB_RECURSE DEV_SOURCES "examples/2_shape_rendering/*.cpp")
# add_executable(2_shape_rendering ${DEV_SOURCES})
# target_include_directories(2_shape_rendering
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
#         ${CMAKE_CURRENT_SOURCE_DIR}/src
# )
# target_link_libraries(2_shape_rendering
#     PRIVATE
#         game_engine
# )
# # Output directory for binaries
# set_target_properties(2_shape_rendering
#     PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#         CXX_SCAN_FOR_MODULES ON 
# )


# file(GLOB_RECURSE DEV_SOURCES "examples/4_user_interface/*.cpp")
# add_executable(4_user_interface ${DEV_SOURCES})
# target_include_directories(4_user_interface
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
#         ${CMAKE_CURRENT_SOURCE_DIR}/src
# )
# target_link_libraries(4_user_interface
#     PRIVATE
#         game_engine
# )
# # Output directory for binaries
# set_target_properties(4_user_interface
#     PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#         CXX_SCAN_FOR_MODULES ON 
# )


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
