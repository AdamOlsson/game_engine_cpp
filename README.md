# Description
This project is a physics engine and render engine to support for my personal game development.

# TODO
- UI elements
    - How to build a UI layout 
    - UI elements should be hoverable
    - UI elements should be clickable
    - UI elements should have text
- Adding new resources should be simple
    - create header file automatically
    - improve finding shaders, can we do this automatically?
    - add shaders to to fetch_all_shaders()
- Can we simplify Pipelines even more?
    - It should be easy to create a new pipeline
- Wrap VkDescriptorSetLayout in a class to handle descruction
- Wrap VkShaderModule in a class to handle descruction
- build the render engine and physics engine separately as with software agent
- Fetch Vulkan during CMake generation of build files
- build project in docker container as software agent at work
- Implement a CPU regexp short

## Examples
- [Example 0](examples/0_collision_detection/): Narrowphase collision detection using SAT
- [Example 1](examples/1_spatial_subdivision/): Broadphase collision detection using Spatial Subdivision 
- [Example 2](examples/2_shape_rendering/): Showcase of the different shapes that can be rendered
- [Example 3](examples/3_collision_detection/): Playground for various collision simulations


## Features To Implement
- UI Rendering
- Procedural generation
- Tiling system
- Camera movement
- Sprite animation
- Procedural animations
- 3D objects
- Lighting


## Refactoring To Perform
- Refactor RigidBody and RigidBuilder to have the ability to be available at compile time
- RenderableGeometry should be rewritten using templates instead of inheritance

## Known bugs
- Example 1 has an issue where solving a collision results in NaN, causing the object to disappear
