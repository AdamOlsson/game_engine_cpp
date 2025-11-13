#version 450

#define CIRCLE 1
#define TRIANGLE 2
#define RECTANGLE 3

struct Border {
    vec4 color;
    float width;
    float radius;
};

struct GeometrySBO {
    mat4 model_matrix;
    vec4 color;
    Border border;
};

layout(binding = 0) readonly buffer GeometryStorageBuffer {
    GeometrySBO instances[]; 
} storage_buffer;

// layout(binding = 1) readonly uniform QuadUniformBuffer {
// } uniform_buffer;

layout(push_constant) uniform CameraMatrix {
    mat4 view_projection;
} pc_camera;

layout(location = 0) in vec3 in_local_position; // -0.5 to 0.5

layout(location = 0) out vec2 out_local_position; // -0.5 to 0.5
layout(location = 1) out vec2 out_geometry_dimensions;
layout(location = 2) out vec4 out_color;
layout(location = 3) out int out_geometry_type;
layout(location = 4) out float out_border_width_px;
layout(location = 5) out float out_border_radius_px;
layout(location = 6) out vec4 out_border_color;

void main() {
    GeometrySBO instance = storage_buffer.instances[gl_InstanceIndex];

    vec4 local_space_position = vec4(in_local_position, 1.0);
    mat4 model_matrix = instance.model_matrix;
    vec4 world_space_position = model_matrix * local_space_position;    
    
    // world space -> view space (using view matrix) then
    // view space -> clip space (using projection matrix)
    vec4 clip_space_position = pc_camera.view_projection * world_space_position;
    
    const float geometry_width_world = instance.model_matrix[0][0];
    const float geometry_height_world = instance.model_matrix[1][1];

    gl_Position = clip_space_position;
    out_local_position = in_local_position.xy;
    out_geometry_dimensions = vec2(geometry_width_world, geometry_height_world);
    out_color = instance.color;
    out_geometry_type = RECTANGLE;
    out_border_width_px = instance.border.width;
    out_border_radius_px = instance.border.radius;
    out_border_color = instance.border.color;
}
