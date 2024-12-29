#version 450
struct InstanceData {
    vec3 position;
    vec3 color;
    float rotation;
};

layout(std140, binding = 0) readonly buffer InstanceDataBlock {
    InstanceData instances[1024];
} instance_data_block;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;

mat3 rotationMatrixZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(c, -s, 0.0,
                s, c, 0.0,
                0.0, 0.0, 1.0);
}

void main() {
    InstanceData instance = instance_data_block.instances[gl_InstanceIndex];
    mat3 rotation_matrix = rotationMatrixZ(instance.rotation);
    vec3 rotated_vertex_pos = rotation_matrix * inPosition; 

    gl_Position = vec4(instance.position + rotated_vertex_pos, 1.0);
    fragColor = instance.color;
}
