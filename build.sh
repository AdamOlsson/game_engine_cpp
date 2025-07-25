#!/bin/bash

# Compile all vertex shaders
echo "Compiling vertex shaders..."
for shader in ./src/render_engine/shaders/vertex/*.glsl; do
    if [ -f "$shader" ]; then
        output="${shader%.glsl}.spv"
        echo "Compiling: $shader -> $output"
        /Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc -fshader-stage=vert "$shader" -o "$output"
        if [ $? -ne 0 ]; then
            echo "Shader compilation failed for: $shader"
            exit 1
        fi
    fi
done

# Compile all fragment shaders
echo "Compiling fragment shaders..."
for shader in ./src/render_engine/shaders/fragment/*.glsl; do
    if [ -f "$shader" ]; then
        output="${shader%.glsl}.spv"
        echo "Compiling: $shader -> $output"
        /Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc -fshader-stage=frag "$shader" -o "$output"
        if [ $? -ne 0 ]; then
            echo "Shader compilation failed for: $shader"
            exit 1
        fi
    fi
done

echo "All shaders compiled successfully!"

python3 compile_assets.py
ASSETS_COMPILE_STATUS=$?

# Check if compile_assets.py was successful
if [ $ASSETS_COMPILE_STATUS -ne 0 ]; then
    echo "Assets compilation failed"
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
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  -DCMAKE_BUILD_TYPE=$BUILD_TYPE && cmake --build build -- -j$(nproc)
