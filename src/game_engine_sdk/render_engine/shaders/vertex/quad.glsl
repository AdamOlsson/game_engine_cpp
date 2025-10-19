#version 450

struct QuadSBO {
    mat4 model_matrix;
};

layout(binding = 0) readonly buffer QuadStorageBuffer {
    QuadSBO instances[]; 
} storage_buffer;

layout(push_constant) uniform CameraMatrix {
    mat4 view_projection;
} pc_camera;

layout(location = 0) in vec3 in_local_position; // -0.5 to 0.5

// layout(location = 0) out vec3 out_clip_position; // -1.0 to 1.0

void main() {
    QuadSBO instance = storage_buffer.instances[gl_InstanceIndex];

    vec4 local_space_position = vec4(in_local_position, 1.0);
    
    mat4 model_matrix = instance.model_matrix;
    vec4 world_space_position = model_matrix * local_space_position;    
    
    // world space -> view space (using view matrix) then
    // view space -> clip space (using projection matrix)
    vec4 clip_space_position = pc_camera.view_projection * world_space_position;

    gl_Position = clip_space_position;
}
