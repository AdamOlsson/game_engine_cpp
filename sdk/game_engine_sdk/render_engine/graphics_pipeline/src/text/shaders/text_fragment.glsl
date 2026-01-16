#version 450

layout(location = 0) in flat int in_is_interior;
layout(location = 1) in vec3 in_uvw;
layout(location = 2) in float in_winding_order;

layout(location = 0) out vec4 out_color; 

void main() {
    bool front_facing = in_winding_order == 0.0; 

    vec3 color = vec3(1.0, 0.0, 0.8);
    if(in_is_interior == 1){
        color = vec3(0.0, 0.4, 0.8);
        out_color = vec4(color, 1.0);
        return;
    } 
    
    bool fill = in_uvw.x * in_uvw.x < in_uvw.y * in_uvw.z;
    if(!front_facing) {
        fill = !fill;
    }

    out_color = vec4(color, fill);
}
