#!/bin/bash

# Compile vertex shader
/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc ./src/render_engine/shaders/shader.vert -o ./src/render_engine/shaders/vert.spv
VERT_COMPILE_STATUS=$?

# Compile fragment shader
/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc ./src/render_engine/shaders/shader.frag -o ./src/render_engine/shaders/frag.spv
FRAG_COMPILE_STATUS=$?

# Check compilation status
if [ $VERT_COMPILE_STATUS -ne 0 ] || [ $FRAG_COMPILE_STATUS -ne 0 ]; then
    echo "Shader compilation failed"
    exit 1
fi

BUILD_TYPE="Debug"

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--debug|--release]"
            exit 1
            ;;
    esac
done

# Create build directory and run CMake
mkdir -p build
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  -DCMAKE_BUILD_TYPE=$BUILD_TYPE && cmake --build build
