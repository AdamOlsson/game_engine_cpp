# Description
This project is a physics engine and render engine to support for my personal game development.

# TODO
- Fetch Vulkan during CMake generation of build files
- Implement a CPU radix sort

## Examples
- [Example 0](examples/0_collision_detection/): Narrowphase collision detection using SAT
- [Example 1](examples/1_spatial_subdivision/): Broadphase collision detection using Spatial Subdivision 
- [Example 2](examples/2_shape_rendering/): Showcase of the different shapes that can be rendered
- [Example 3](examples/3_collision_detection/): Playground for various collision simulations


## Features To Implement
- Procedural generation
- Tiling system
- Camera movement
- Sprite animation
- Procedural animations
- 3D objects
- Lighting


## Known bugs
- Example 1 has an issue where solving a collision results in NaN, causing the object to disappear
