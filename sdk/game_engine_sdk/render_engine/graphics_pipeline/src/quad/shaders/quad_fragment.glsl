#version 450
#extension GL_EXT_nonuniform_qualifier : require 

#define SAMPLING_MODE_DEFAULT 0
#define SAMPLING_MODE_SDF 1

layout(binding = 2) uniform sampler2D u_texture_sampler[];

layout(location = 0) in vec2 in_uv;
layout(location = 1) in flat uint in_texture_id;
layout(location = 2) in flat vec4 in_color;
layout(location = 3) in flat uint in_sampling_mode;

layout(location = 0) out vec4 out_color; 

void main() {
    if (in_uv.x < 0.0 || in_uv.y < 0.0) {
        out_color = in_color; 
        return;
    }
    
    vec4 color;
    if(in_sampling_mode == SAMPLING_MODE_DEFAULT){
        color = texture(u_texture_sampler[nonuniformEXT(in_texture_id)], in_uv);
    } else if(in_sampling_mode == SAMPLING_MODE_SDF){
        vec4 rgba = texture(u_texture_sampler[nonuniformEXT(in_texture_id)], in_uv);
        color = vec4(in_color.xyz, rgba.a);
    } else {
        discard;
    }

    out_color = color;
}
