#version 450

layout(location = 0) in vec4 frag_color;
layout(location = 1) in vec2 uv;
layout(location = 2) in float font_weight;
layout(binding = 2) uniform sampler2D texture_sampler;

layout(location = 0) out vec4 out_color;

void main() {
    if (uv.x < 0.0 || uv.y < 0.0) {
        out_color = frag_color; // Full opacity
        return;
    }
    
    float distance = texture(texture_sampler, uv).r;
    float smooth_width = fwidth(distance);
    float alpha = smoothstep(font_weight - smooth_width, font_weight + smooth_width, distance);
    
    out_color = vec4(frag_color.rgb, alpha);
}
