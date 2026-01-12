# ============================================================================
# File: cmake/FindTTFParser.cmake
# ============================================================================
# This module finds and builds the ttf-parser Rust library
# 
# Output variables:
#   TTFParser_FOUND       - True if ttf-parser is found/built successfully
#   TTFParser_INCLUDE_DIR - Include directory for ttf-parser headers
#   TTFParser_LIBRARY     - Path to the ttf-parser library
#
# Imported targets:
#   TTFParser::ttfparser  - The ttf-parser library target
# ============================================================================

# Check if Cargo is installed
find_program(CARGO_EXECUTABLE cargo)
if(NOT CARGO_EXECUTABLE)
    message(FATAL_ERROR "Cargo not found. Please install Rust from https://rustup.rs/")
endif()

# Check if Git is installed (for cloning)
find_package(Git REQUIRED)

# Set the path to ttf-parser
set(TTF_PARSER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ttf-parser")
set(TTF_PARSER_C_API_DIR "${TTF_PARSER_DIR}/c-api")

# Clone ttf-parser if it doesn't exist
if(NOT EXISTS "${TTF_PARSER_DIR}/.git")
    message(STATUS "ttf-parser not found. Cloning from GitHub...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --depth 1 
                https://github.com/harfbuzz/ttf-parser.git
                ${TTF_PARSER_DIR}
        RESULT_VARIABLE GIT_RESULT
        OUTPUT_VARIABLE GIT_OUTPUT
        ERROR_VARIABLE GIT_ERROR
    )
    
    if(NOT GIT_RESULT EQUAL 0)
        message(FATAL_ERROR 
            "Failed to clone ttf-parser:\n${GIT_ERROR}")
    endif()
    
    message(STATUS "ttf-parser cloned successfully")
endif()

# Verify the c-api directory exists
if(NOT EXISTS "${TTF_PARSER_C_API_DIR}/Cargo.toml")
    message(FATAL_ERROR 
        "ttf-parser c-api not found at ${TTF_PARSER_C_API_DIR}. "
        "The repository may be corrupted. Try deleting ${TTF_PARSER_DIR} and rebuilding.")
endif()

# Determine build profile and output directory
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_BUILD_TYPE "debug")
    set(CARGO_BUILD_FLAG "")
else()
    set(CARGO_BUILD_TYPE "release")
    set(CARGO_BUILD_FLAG "--release")
endif()

set(TTF_PARSER_BUILD_DIR "${TTF_PARSER_C_API_DIR}/target/${CARGO_BUILD_TYPE}")

# Determine library name based on platform
if(WIN32)
    set(TTF_PARSER_LIB_NAME "ttfparser.lib")
    set(TTF_PARSER_DLL_NAME "ttfparser.dll")
elseif(APPLE)
    set(TTF_PARSER_LIB_NAME "libttfparser.dylib")
elseif(UNIX)
    set(TTF_PARSER_LIB_NAME "libttfparser.dylib")
endif()

set(TTF_PARSER_LIB_PATH "${TTF_PARSER_BUILD_DIR}/${TTF_PARSER_LIB_NAME}")

# Custom command to build the Rust library
add_custom_command(
    OUTPUT ${TTF_PARSER_LIB_PATH}
    COMMAND ${CARGO_EXECUTABLE} build ${CARGO_BUILD_FLAG} --lib
    WORKING_DIRECTORY ${TTF_PARSER_C_API_DIR}
    COMMENT "Building ttf-parser Rust library (${CARGO_BUILD_TYPE} mode)..."
    VERBATIM
)

# Create a custom target for the Rust library
add_custom_target(ttfparser_build
    DEPENDS ${TTF_PARSER_LIB_PATH}
)

# Create an imported library target
add_library(TTFParser::ttfparser STATIC IMPORTED GLOBAL)
set_target_properties(TTFParser::ttfparser PROPERTIES
    IMPORTED_LOCATION ${TTF_PARSER_LIB_PATH}
    INTERFACE_INCLUDE_DIRECTORIES ${TTF_PARSER_C_API_DIR}
)

# Make sure the Rust library is built before linking
add_dependencies(TTFParser::ttfparser ttfparser_build)

# On some platforms, you may need to link additional system libraries
if(UNIX AND NOT APPLE)
    # Link pthread and dl on Linux
    target_link_libraries(TTFParser::ttfparser INTERFACE pthread dl m)
elseif(APPLE)
    # Link system framework on macOS
    target_link_libraries(TTFParser::ttfparser INTERFACE "-framework CoreFoundation" "-framework Security")
elseif(WIN32)
    # Link Windows system libraries
    target_link_libraries(TTFParser::ttfparser INTERFACE ws2_32 userenv bcrypt ntdll)
endif()

# Set output variables
set(TTFParser_FOUND TRUE)
set(TTFParser_INCLUDE_DIR ${TTF_PARSER_C_API_DIR})
set(TTFParser_LIBRARY ${TTF_PARSER_LIB_PATH})

message(STATUS "Found ttf-parser: ${TTF_PARSER_DIR}")
message(STATUS "  Include dir: ${TTFParser_INCLUDE_DIR}")
message(STATUS "  Library: ${TTF_PARSER_LIB_PATH}")
