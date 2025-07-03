#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uv;
layout(binding = 3) uniform sampler2D texture_sampler;

layout(location = 0) out vec4 outColor;

void main() {
    if (uv.x >= 0.0 && uv.y >= 0.0) {
        vec4 pixel_color = texture(texture_sampler, uv); 
        // Can check any of the values 
        if (pixel_color.x < 0.5f) {
            outColor = vec4(0.0,0.0,0.0,0.0);
        } else {
            outColor = vec4(1.0,1.0,1.0,1.0);
        }
    } else {
        outColor = vec4(fragColor, 1.0);
    }
}
