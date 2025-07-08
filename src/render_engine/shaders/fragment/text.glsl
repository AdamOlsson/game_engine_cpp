#version 450

layout(location = 0) in vec4 frag_color;
layout(location = 1) in vec2 uv;
layout(location = 2) in float font_weight;
layout(location = 3) in float font_sharpness;
layout(binding = 2) uniform sampler2D texture_sampler;

layout(location = 0) out vec4 out_color;

void main() {
    if (uv.x >= 0.0 && uv.y >= 0.0) {
        float distance = texture(texture_sampler, uv).x;  // SDF distance

        float threshold = 0.5 - (font_weight - 0.5) * 0.2; // Adjust 0.2 to control weight range
        float smooth_width = fwidth(distance) / font_sharpness;

        float alpha = smoothstep(threshold - smooth_width, threshold + smooth_width, distance);
        out_color = vec4(frag_color.rgb, alpha);
    } else {
        out_color = vec4(frag_color.rgb, 1.0);
    }
}
