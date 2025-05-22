#!/bin/bash

# Compile vertex shader
/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc ./src/render_engine/shaders/shader.vert -o ./src/render_engine/shaders/vert.spv
VERT_COMPILE_STATUS=$?

# Compile geometry fragment shader
/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc ./src/render_engine/shaders/geometry.frag -o ./src/render_engine/shaders/geometry_fragment.spv
GEOMETRY_FRAG_COMPILE_STATUS=$?

# Compile text fragment shader
/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc ./src/render_engine/shaders/text.frag -o ./src/render_engine/shaders/text_fragment.spv
TEXT_FRAG_COMPILE_STATUS=$?

# Compile ui shaders
/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc -fshader-stage=vert ./src/render_engine/shaders/ui_vertex.glsl -o ./src/render_engine/shaders/ui_vertex.spv
UI_VERT_COMPILE_STATUS=$?

/Users/adamolsson/VulkanSDK/1.3.296.0/macOS/bin/glslc -fshader-stage=frag ./src/render_engine/shaders/ui_fragment.glsl -o ./src/render_engine/shaders/ui_fragment.spv
UI_FRAG_COMPILE_STATUS=$?



# Check compilation status
if [ $VERT_COMPILE_STATUS -ne 0 ] ||
   [ $GEOMETRY_FRAG_COMPILE_STATUS -ne 0 ] ||
   [ $UI_VERT_COMPILE_STATUS -ne 0 ] ||
   [ $UI_FRAG_COMPILE_STATUS -ne 0 ] ||
   [ $TEXT_FRAG_COMPILE_STATUS -ne 0 ]; then
    echo "Shader compilation failed"
    exit 1
fi

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
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  -DCMAKE_BUILD_TYPE=$BUILD_TYPE && cmake --build build -- -j4
