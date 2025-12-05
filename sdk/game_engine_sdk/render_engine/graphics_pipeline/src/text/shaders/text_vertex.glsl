#version 450

struct TextSBO {
    mat4 model_matrix;
};

layout(binding = 0) readonly buffer TextStorageBuffer {
    TextSBO instances[]; 
} storage_buffer;

// layout(binding = 1) readonly uniform TextUniformBuffer {
// } uniform_buffer;

layout(push_constant) uniform CameraMatrix {
    mat4 view_projection;
} pc_camera;

layout(location = 0) in vec3 in_world_position;


void main() {
    TextSBO instance = storage_buffer.instances[gl_InstanceIndex];

    gl_Position = pc_camera.view_projection * instance.model_matrix * vec4(in_world_position, 1.0);
    gl_PointSize = 10.0;
}
