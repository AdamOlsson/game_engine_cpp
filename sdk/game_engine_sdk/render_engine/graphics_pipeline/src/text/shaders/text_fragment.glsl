#version 450


layout(location = 0) out vec4 out_color; 

void main() {
    
    float distance = length(gl_PointCoord - vec2(0.5));
    
    if(distance < 0.5) {
        out_color = vec4(0.0,0.8,0.8,1.0);
    } else {
        out_color = vec4(0.0,0.8,0.8,0.0);
    }
}
