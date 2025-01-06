#version 450

#define PI 3.14159265

struct InstanceData {
    vec3 position;
    vec3 color;
    float rotation;
    float side;
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

vec3 scale_vertex(vec3 vertex, float side) {
    float original_length = length(vertex);
    float half_side = side / 2.0;
    float new_length = half_side / cos(PI / 6.0);
    float scale = new_length / original_length;
    return vertex*scale;
}

void main() {
    InstanceData instance = instance_data_block.instances[gl_InstanceIndex];

    vec3 scaled_vertex_pos = scale_vertex(inPosition, instance.side);
    mat3 rotation_matrix = rotationMatrixZ(instance.rotation);
    vec3 rotated_vertex_pos = rotation_matrix * scaled_vertex_pos; 

    gl_Position = vec4(instance.position + rotated_vertex_pos, 1.0);
    fragColor = instance.color;
}
