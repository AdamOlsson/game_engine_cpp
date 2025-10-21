#version 450
#extension GL_EXT_nonuniform_qualifier : require 

layout(binding = 2) uniform sampler2D u_texture_sampler[];

layout(location = 0) in vec2 in_uv;
layout(location = 1) in flat uint in_texture_id;

layout(location = 0) out vec4 out_color; 

void main() {
    if ( in_uv.x < 0.0 || in_uv.y < 0.0) {
        out_color = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    out_color = texture(u_texture_sampler[nonuniformEXT(in_texture_id)], in_uv);
}
