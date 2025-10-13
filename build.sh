#!/bin/bash

# Compile all vertex shaders
echo "Compiling vertex shaders..."
for shader in ./src/game_engine_sdk/render_engine/shaders/vertex/*.glsl; do
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
for shader in ./src/game_engine_sdk/render_engine/shaders/fragment/*.glsl; do
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


BUILD_TYPE="debug"

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="debug"
            shift
            ;;
        --release)
            BUILD_TYPE="release"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--debug|--release]"
            exit 1
            ;;
    esac
done

BUILD_DIR="build/${BUILD_TYPE}"
# Create build directory and run CMake
mkdir -p "$BUILD_DIR"
cmake -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DGAME_ENGINE_SDK_BUILD_TEST=TRUE\
    -DGAME_ENGINE_SDK_BUILD_EXAMPLES=TRUE
cmake --build "$BUILD_DIR" -- -j$(nproc)
