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
layout(location = 1) in vec3 in_uvw;

layout(location = 0) out int out_is_interior;
layout(location = 1) out vec3 out_uvw;
layout(location = 2) out float out_winding_order;

int is_interior(vec3 uvw) {
    return uvw.x == 0.0 && uvw.y == 0.0 && uvw.z == 0.0 ? 1 : 0;
}

void main() {
    TextSBO instance = storage_buffer.instances[gl_InstanceIndex];

    gl_Position = pc_camera.view_projection * instance.model_matrix * vec4(in_world_position.xy, 0.0, 1.0);
    out_is_interior = is_interior(in_uvw);
    out_uvw = in_uvw;
    out_winding_order = in_world_position.z;
}
