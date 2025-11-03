cmake_minimum_required(VERSION 3.28)

message(STATUS "Configuring render_engine modules")

project(render_engine)

option(RENDER_ENGINE_BUILD_IMAGE "Build the image module" ON)
option(RENDER_ENGINE_BUILD_CAMERA "Build the camera module" ON)
option(RENDER_ENGINE_BUILD_TILING "Build the tiling module" ON)
option(RENDER_ENGINE_BUILD_WINDOW "Build the window module" ON)
option(RENDER_ENGINE_BUILD_VULKAN "Build the vulkan module" ON)

if(RENDER_ENGINE_BUILD_IMAGE)
    message(STATUS "Adding image module")
    add_subdirectory(image)
else()
    message(STATUS "Skipping image module")
endif()

if(RENDER_ENGINE_BUILD_CAMERA)
    message(STATUS "Adding camera module")
    add_subdirectory(camera)
else()
    message(STATUS "Skipping camera module")
endif()

if(RENDER_ENGINE_BUILD_TILING)
    message(STATUS "Adding tiling module")
    add_subdirectory(tiling)
else()
    message(STATUS " Skipping tiling module")
endif()

if(RENDER_ENGINE_BUILD_WINDOW)
    message(STATUS "Adding window module")
    add_subdirectory(window)
else()
    message(STATUS " Skipping window module")
endif()

if(RENDER_ENGINE_BUILD_VULKAN)
    message(STATUS "Adding vulkan module")
    add_subdirectory(vulkan)
else()
    message(STATUS " Skipping vulkan module")
endif()

