cmake_minimum_required(VERSION 3.28)

project(5_snake)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(GAME_ENGINE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../build/lib")

# TODO: Bundle vulkan and GLFW with the SDK
set(GAME_ENGINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(GAME_ENGINE_SDK_DIR "${GAME_ENGINE_ROOT_DIR}/build/release/lib")
set(GAME_ENGINE_INCLUDE_DIR "${GAME_ENGINE_ROOT_DIR}/include")

set(VULKAN_SDK_PATH "/Users/adamolsson/VulkanSDK/1.3.296.0/macOS")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK_PATH}/include")
set(Vulkan_LIBRARY "${VULKAN_SDK_PATH}/lib/libvulkan.1.3.296.dylib")

IF(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()

find_package(glfw3 REQUIRED)


file(GLOB_RECURSE SNAKE_SOURCES "src/*.cpp")
add_executable(5_snake ${SNAKE_SOURCES})

target_compile_features(5_snake PUBLIC cxx_std_20)

target_include_directories(5_snake
    PRIVATE
        ${GAME_ENGINE_INCLUDE_DIR}
        ${Vulkan_INCLUDE_DIR}
)

target_link_libraries(5_snake
    PRIVATE
        "${GAME_ENGINE_SDK_DIR}/libgame_engine.a"  # Direct path to .a file
        ${Vulkan_LIBRARY}
        glfw

)

# Output directory for binaries
set_target_properties(5_snake
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
