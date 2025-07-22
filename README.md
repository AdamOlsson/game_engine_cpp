# Description
This project is a physics engine and render engine to support for my personal game development.

# TODO
- Can we simplify Pipelines even more?
    - It should be easy to create a new pipeline
- Wrap VkDescriptorSetLayout in a class to handle descruction
- Wrap DescriptorSet, DescriptorSetLayout and DescriptorPool in a single class
- Wrap VkShaderModule in a class to handle descruction
- build the render engine and physics engine separately as with software agent
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


## Refactoring To Perform
- Refactor RigidBody and RigidBuilder to have the ability to be available at compile time

## Known bugs
- Example 1 has an issue where solving a collision results in NaN, causing the object to disappear
