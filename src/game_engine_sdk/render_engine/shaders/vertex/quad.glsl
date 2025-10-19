#version 450

layout(push_constant) uniform CameraMatrix {
    mat4 view_projection;
} pc_camera;

layout(location = 0) in vec3 in_local_position; // -0.5 to 0.5

// layout(location = 0) out vec3 out_clip_position; // -1.0 to 1.0

void main() {
    
    vec4 local_space_position = vec4(in_local_position, 1.0);
    
    mat4 model_matrix = mat4(
        100.0, 0.0, 0.0, 0.0,  
        0.0, 100.0, 0.0, 0.0,  
        0.0, 0.0, 1.0, 0.0,    
        0.0, 0.0, 0.0, 1.0     
    );
    vec4 world_space_position = model_matrix * local_space_position;    
    
    // world space -> view space (using view matrix) then
    // view space -> clip space (using projection matrix)
    vec4 clip_space_position = pc_camera.view_projection * world_space_position;

    gl_Position = clip_space_position;
}
