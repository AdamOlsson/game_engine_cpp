#version 450

layout(std140, binding = 0) readonly buffer StorageBufferObject {
    vec3 position;
    vec3 color;
    float rotation;
} ssbo;

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
    mat3 rotation_matrix = rotationMatrixZ(ssbo.rotation);
    vec3 rotated_vertex_pos = rotation_matrix * inPosition; 

    gl_Position = vec4(ssbo.position + rotated_vertex_pos, 1.0);
    fragColor = ssbo.color;
}
