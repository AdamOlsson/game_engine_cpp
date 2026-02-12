# Game Engine C++ - Agent Guidelines

This document contains essential information for agentic coding agents working in this C++ game engine repository.

## Build System & Commands

### Primary Build System: CMake 3.28+
```bash
# Configure build (Debug)
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug

# Configure with tests
cmake -S . -B build/debug -DGAME_ENGINE_SDK_BUILD_TEST=ON -DCMAKE_BUILD_TYPE=Debug

# Configure with examples  
cmake -S . -B build/debug -DGAME_ENGINE_SDK_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug

# Build project
cmake --build build/debug

# Build specific targets
cmake --build build/debug --target unit_tests
cmake --build build/debug --target render_engine_tests
```

### Testing Commands
```bash
# Run all tests (from build directory)
cd build/debug
ctest --output-on-failure

# Run specific test suites
./bin/tests/unit_tests
./bin/tests/render_engine_tests

# Run single test (use --gtest_filter)
./bin/tests/unit_tests --gtest_filter="Vector2Tests.Test_AxisReference"
```

### Key CMake Options
- `GAME_ENGINE_SDK_BUILD_TEST=ON/OFF`: Enable unit tests
- `GAME_ENGINE_SDK_BUILD_EXAMPLES=ON/OFF`: Build example programs
- `CMAKE_LOG_LEVEL_DEBUG=ON/OFF`: Enable debug logging

## Code Style Guidelines

### Formatting (.clang-format)
- **Style**: LLVM-based
- **Indentation**: 4 spaces
- **Column Limit**: 90 characters
- **Header Guards**: Use `#pragma once`

### Naming Conventions
- **Classes**: `PascalCase` (e.g., `GameEngine`, `Vector2`, `Matrix`)
- **Member Variables**: `m_` prefix with snake_case (e.g., `m_start_tick`, `m_matrix`)
- **Functions**: `snake_case` (e.g., `run()`, `translate()`, `rotate_z()`)
- **Namespaces**: `lowercase` (e.g., `math`, `vulkan::context`)
- **Constants**: `UPPER_SNAKE_CASE` (e.g., `GAME_ENGINE_SDK_BUILD_TEST`)
- **Files**: `PascalCase.h` for headers, `PascalCase.cpp` for sources

### C++ Standards
- **C++ Standard**: C++20 (required)
- **Headers**: Include project headers first, then system headers
- **Smart Pointers**: Prefer `std::unique_ptr` and `std::shared_ptr` over raw pointers
- **RAII**: Consistent resource management with destructors
- **Const Correctness**: Use `const` wherever possible

### Import/Include Organization
```cpp
// Project headers (use relative paths from include directory)
#include "math/Vector2.h"
#include "render_engine/Window.h"

// Third-party libraries  
#include <glm/glm.hpp>
#include <gtest/gtest.h>

// Standard library
#include <memory>
#include <vector>
```

## Project Architecture

### Directory Structure & Phase-out Plan
- **Active Development**: `sdk/game_engine_sdk/` - Main modular SDK
- **Phase-out Directories** (DO NOT MODIFY):
  - `src/` and `include/` - Legacy code being migrated to SDK
  - `tests/` - Legacy tests being migrated to SDK modules
  - `releases/` - Previous SDK versions (READ-ONLY)
  - `prototypes/` - Experimental code (READ-ONLY)

### Core SDK Modules (`sdk/game_engine_sdk/`)
- **math/**: Mathematical utilities (vectors, matrices, interpolation)
- **logger/**: Logging system
- **util/**: General utilities  
- **ads/**: Abstract data structures
- **render_engine/**: Rendering subsystem
- **Examples**: `examples/` directory with individual demos
- **Tests**: Each module has `test/` subdirectory

### Testing Guidelines
- **Framework**: Google Test (GTest)
- **Test Files**: End with `_test.cpp`
- **Test Naming**: `TEST(TestSuite, TestDescription)` format
- **Assertions**: Use `EXPECT_*` for non-fatal, `ASSERT_*` for fatal failures
- **Floating Point**: Use `EXPECT_NEAR` for float comparisons with tolerance

### Error Handling
- **Exceptions**: Use sparingly, prefer error codes for recoverable errors
- **Assertions**: Use `assert()` for internal invariants (debug builds only)
- **Logging**: Use the logger module for runtime error reporting
- **Return Values**: Use `std::optional<T>` or `std::expected<T, E>` where appropriate

### Performance Guidelines
- **Move Semantics**: Use `std::move` for transferring ownership
- **Pass by Reference**: Pass large objects by `const&`, modify by reference
- **Template Usage**: Use templates for generic algorithms, avoid code bloat
- **Memory Management**: RAII with smart pointers, avoid manual `new`/`delete`

## Development Workflow

### Adding New Features
1. Create module in appropriate SDK directory or extend existing module
2. Add header to `public/` or appropriate include directory
3. Add implementation to `src/` directory
4. Add tests in `test/` subdirectory
5. Update CMakeLists.txt if adding new source files
6. Run tests to verify functionality

### Directory Restrictions
- **DO NOT MODIFY** files in `src/`, `include/`, `tests/` (being phased out)
- **DO NOT MODIFY** files in `releases/` (previous SDK versions, read-only)
- **DO NOT MODIFY** files in `prototypes/` (experimental code, read-only)
- **ONLY WORK** in `sdk/game_engine_sdk/` for active development

### Debugging
- **Debug Builds**: Always use `-DCMAKE_BUILD_TYPE=Debug` for development
- **Logging**: Enable with `-DCMAKE_LOG_LEVEL_DEBUG=ON`
- **Sanitizers**: Add `-DCMAKE_CXX_FLAGS_DEBUG="-g -fsanitize=address"` if needed

### Vulkan Integration
- **Context Management**: Use `vulkan::context` namespace
- **Resource Cleanup**: Vulkan objects require explicit cleanup - use RAII wrappers
- **Validation Layers**: Enable in debug builds for validation

## Important Notes

- **Dependencies**: Vulkan SDK, GLFW, GLM, and Google Test must be installed
- **Platform**: Currently targeting desktop platforms (Linux, Windows, macOS)
- **Graphics**: Vulkan-based rendering pipeline
- **Physics**: Custom physics engine with collision detection and response
- **ECS**: Entity-Component System for game object management

Always run tests before submitting changes and ensure the build completes without warnings.