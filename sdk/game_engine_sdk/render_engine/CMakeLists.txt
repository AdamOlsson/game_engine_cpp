cmake_minimum_required(VERSION 3.28)

message(STATUS "Configuring render_engine modules")

project(render_engine)


option(RENDER_ENGINE_BUILD_IMAGE "Build the image module" ON)
option(RENDER_ENGINE_BUILD_TILING "Build the tiling module" ON)

if(RENDER_ENGINE_BUILD_IMAGE)
    message(STATUS "Adding image module")
    add_subdirectory(image)
else()
    message(STATUS "Skipping image module")
endif()

if(RENDER_ENGINE_BUILD_TILING)
    message(STATUS "Adding tiling module")
    add_subdirectory(tiling)
else()
    message(STATUS " Skipping tiling module")
endif()


