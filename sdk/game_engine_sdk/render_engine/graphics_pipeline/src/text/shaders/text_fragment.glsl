#version 450
layout(location = 0) in flat int in_is_interior;
layout(location = 1) in vec3 in_uvw;
layout(location = 2) in float in_winding_order;
layout(location = 0) out vec4 out_color; 

void main() {
    vec3 color = vec3(1.0, 0.0, 0.8);
    
    // Handle filled polygon interiors
    if(in_is_interior == 1){
        color = vec3(0.0, 0.4, 0.8);
        out_color = vec4(color, 1.0);
        return;
    } 
    
    // Quadratic Bézier implicit equation: f = u² - v*w
    // f < 0 means we're on the RIGHT side of the curve
    // f > 0 means we're on the LEFT side of the curve
    float f = in_uvw.x * in_uvw.x - in_uvw.y * in_uvw.z;
    
    // winding_order follows FreeType convention:
    // 0.0 (FontFill::Right) = fill right side -> keep when f < 0
    // 1.0 (FontFill::Left) = fill left side -> keep when f > 0
    bool fill;

    if(in_winding_order < 0.5) {
        // FontFill::Right: fill the right side of the curve
        fill = f < 0.0;
    } else {
        // FontFill::Left: fill the left side of the curve
        fill = f > 0.0;
    }
    
    if(!fill) {
        discard;
    }
    
    out_color = vec4(color, 1.0);
}
