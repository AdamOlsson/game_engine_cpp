#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 uv;
layout(binding = 2) uniform sampler2D texture_sampler;
layout(binding = 3) readonly uniform VertexData {
        vec3 vertices[];
        int num_vertices;
        int max_vertices;
} vertices;

layout(location = 0) out vec4 outColor;

void main() {
    if (uv.x >= 0.0 && uv.y >= 0.0) {
        outColor = texture(texture_sampler, uv); 
    } else {
        outColor = fragColor;
    }
}
