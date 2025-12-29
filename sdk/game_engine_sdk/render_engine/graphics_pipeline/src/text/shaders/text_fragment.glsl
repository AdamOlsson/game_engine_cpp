#version 450

layout(location = 0) in flat int is_interior;

layout(location = 0) out vec4 out_color; 

void main() {
   
    if(is_interior == 1){
        out_color = vec4(0.0,0.8,0.8,1.0);
    } else {
        out_color = vec4(1.0,1.0,1.0,1.0);
    }

}
