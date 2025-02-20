# Description
This project is a physics engine and render engine to support for my personal game development.

## Examples
- [Example 0](examples/0_collision_detection/): Narrowphase collision detection using SAT
- [Example 1](examples/1_spatial_subdivision/): Broadphase collision detection using Spatial Subdivision 
- [Example 2](examples/2_shape_rendering/): Showcase of the different shapes that can be rendered
- [Example 3](examples/3_collision_detection/): Playground for various collision simulations


## Features To Implement
- SAT circle-circle, circle-polygon collision detection
- Text rendering
- Texture rendering
- UI Rendering
- Procedural generation
- Tiling system
- Camera movement
- Sprite animation
- Procedural animations
- 3D objects
- Lighting
- Texture rendering


## Refactoring To Perform
- Refactor RigidBody and RigidBuilder to have the ability to be available at compile time
- RenderableGeometry should be rewritten using templates instead of inheritance

## Bugs
- Example 1 has an issue where solving a collision results in NaN, causing the object to disappear
