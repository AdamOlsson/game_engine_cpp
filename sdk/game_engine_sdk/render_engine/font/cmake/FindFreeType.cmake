# ============================================================================
# File: cmake/FindFreeType.cmake
# ============================================================================
# This module finds and builds the FreeType library from source
# 
# Output variables:
#   FreeType_FOUND       - True if FreeType is found/built successfully
#   FreeType_INCLUDE_DIR - Include directory for FreeType headers
#   FreeType_LIBRARY     - Path to the FreeType library
#
# Imported targets:
#   FreeType::FreeType   - The FreeType library target
# ============================================================================

# Check if Git is installed (for cloning)
find_package(Git REQUIRED)

# Set the path to FreeType
set(FREETYPE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype")
set(FREETYPE_BUILD_DIR "${FREETYPE_DIR}/build")

# Clone FreeType if it doesn't exist
if(NOT EXISTS "${FREETYPE_DIR}/.git")
    message(STATUS "FreeType not found. Cloning from GitHub...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --depth 1 
                https://github.com/freetype/freetype.git
                ${FREETYPE_DIR}
        RESULT_VARIABLE GIT_RESULT
        OUTPUT_VARIABLE GIT_OUTPUT
        ERROR_VARIABLE GIT_ERROR
    )
    
    if(NOT GIT_RESULT EQUAL 0)
        message(FATAL_ERROR 
            "Failed to clone FreeType:\n${GIT_ERROR}")
    endif()
    
    message(STATUS "FreeType cloned successfully")
endif()

# Verify the repository
if(NOT EXISTS "${FREETYPE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR 
        "FreeType CMakeLists.txt not found at ${FREETYPE_DIR}. "
        "The repository may be corrupted. Try deleting ${FREETYPE_DIR} and rebuilding.")
endif()

# Configure FreeType build options
set(FT_DISABLE_ZLIB ON CACHE BOOL "Disable zlib support" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable bzip2 support" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "Disable PNG support" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable HarfBuzz support" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable Brotli support" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library" FORCE)

# Determine library name based on platform and build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FREETYPE_LIB_SUFFIX "d")
else()
    set(FREETYPE_LIB_SUFFIX "")
endif()

if(WIN32)
    set(FREETYPE_LIB_NAME "freetype${FREETYPE_LIB_SUFFIX}.lib")
elseif(APPLE OR UNIX)
    set(FREETYPE_LIB_NAME "libfreetype${FREETYPE_LIB_SUFFIX}.a")
endif()

set(FREETYPE_LIB_PATH "${FREETYPE_BUILD_DIR}/${FREETYPE_LIB_NAME}")

# Create build directory if it doesn't exist
file(MAKE_DIRECTORY ${FREETYPE_BUILD_DIR})

# Check if library already exists
if(NOT EXISTS ${FREETYPE_LIB_PATH})
    message(STATUS "Building FreeType static library...")
    
    # Configure FreeType with CMake
    execute_process(
        COMMAND ${CMAKE_COMMAND} 
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DBUILD_SHARED_LIBS=OFF
                -DFT_DISABLE_ZLIB=ON
                -DFT_DISABLE_BZIP2=ON
                -DFT_DISABLE_PNG=ON
                -DFT_DISABLE_HARFBUZZ=ON
                -DFT_DISABLE_BROTLI=ON
                -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                ..
        WORKING_DIRECTORY ${FREETYPE_BUILD_DIR}
        RESULT_VARIABLE CONFIG_RESULT
        OUTPUT_VARIABLE CONFIG_OUTPUT
        ERROR_VARIABLE CONFIG_ERROR
    )
    
    if(NOT CONFIG_RESULT EQUAL 0)
        message(FATAL_ERROR 
            "Failed to configure FreeType:\n${CONFIG_ERROR}")
    endif()
    
    # Build FreeType
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${FREETYPE_BUILD_DIR}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_VARIABLE BUILD_OUTPUT
        ERROR_VARIABLE BUILD_ERROR
    )
    
    if(NOT BUILD_RESULT EQUAL 0)
        message(FATAL_ERROR 
            "Failed to build FreeType:\n${BUILD_ERROR}")
    endif()
    
    message(STATUS "FreeType built successfully")
endif()

# Find the actual library file (it might be in a subdirectory)
file(GLOB_RECURSE FREETYPE_LIB_FOUND 
    "${FREETYPE_BUILD_DIR}/libfreetype*.a"
    "${FREETYPE_BUILD_DIR}/freetype*.lib"
)

if(FREETYPE_LIB_FOUND)
    list(GET FREETYPE_LIB_FOUND 0 FREETYPE_LIB_PATH)
else()
    message(FATAL_ERROR "Could not find built FreeType library in ${FREETYPE_BUILD_DIR}")
endif()

# Create an imported library target
add_library(FreeType::FreeType STATIC IMPORTED GLOBAL)
set_target_properties(FreeType::FreeType PROPERTIES
    IMPORTED_LOCATION ${FREETYPE_LIB_PATH}
    INTERFACE_INCLUDE_DIRECTORIES "${FREETYPE_DIR}/include"
)

# Link system libraries based on platform
if(UNIX AND NOT APPLE)
    # Linux may need pthread and m (math)
    target_link_libraries(FreeType::FreeType INTERFACE m pthread)
elseif(APPLE)
    # macOS typically doesn't need extra libraries for FreeType
    # But you might need CoreFoundation for some features
    # target_link_libraries(FreeType::FreeType INTERFACE "-framework CoreFoundation")
elseif(WIN32)
    # Windows system libraries
    target_link_libraries(FreeType::FreeType INTERFACE)
endif()

# Set output variables
set(FreeType_FOUND TRUE)
set(FreeType_INCLUDE_DIR "${FREETYPE_DIR}/include")
set(FreeType_LIBRARY ${FREETYPE_LIB_PATH})

message(STATUS "Found FreeType: ${FREETYPE_DIR}")
message(STATUS "  Include dir: ${FreeType_INCLUDE_DIR}")
message(STATUS "  Library: ${FREETYPE_LIB_PATH}")
