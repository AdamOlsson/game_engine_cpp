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

# Create build directory and run CMake
mkdir -p build
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build build
