#version 450

#define PI 3.14159265
#define CIRCLE 1
#define TRIANGLE 2
#define RECTANGLE 3
#define HEKTAGON 4
#define ARROW 5 
#define LINE 6 

struct Shape {
    float param1;
    float param2;
};

struct InstanceData {
    vec3 position;
    vec3 color;
    float rotation;
    uint shape_type;
    Shape shape;
};

layout(std140, binding = 0) readonly buffer InstanceDataBlock {
    InstanceData instances[1024];
} instance_data_block;

layout(binding = 1) readonly uniform WindowDimensions {
        vec2 dims;
} window;

layout(push_constant) uniform ShapeType {
    uint id;
} target_shape_type; 

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;

mat3 rotationMatrixZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(c, -s, 0.0,
                s, c, 0.0,
                0.0, 0.0, 1.0);
}

vec3 scale_vertex(vec3 vertex, float width, float height) {
    return vec3(
        vertex.x * width,
        vertex.y * height,
        vertex.z
    );
}

vec2 position_to_uv(vec2 pixel_pos, vec2 window_dims) {
    return vec2(
        (2.0 * pixel_pos.x) / window_dims.x,
        -(2.0 * pixel_pos.y) / window_dims.y
    );
}

void main() {
    InstanceData instance = instance_data_block.instances[gl_InstanceIndex];
    
    vec4 position = vec4(2.0, 2.0, 0.0, 1.0);  // Position far outside visible area
    vec3 color = vec3(0.0);
    
    if (instance.shape_type == target_shape_type.id) {
        vec3 scaled_vertex_pos;
        
        switch(instance.shape_type) {
            case CIRCLE:
            case TRIANGLE:
            case HEKTAGON:
                scaled_vertex_pos = scale_vertex(inPosition, instance.shape.param1, instance.shape.param1);
                break;
            case RECTANGLE:
                scaled_vertex_pos = scale_vertex(inPosition, instance.shape.param1, instance.shape.param2);
                break;
            default:
                scaled_vertex_pos = inPosition;  // Fallback to original position
        }
        
        mat3 rotation_matrix = rotationMatrixZ(-instance.rotation);
        vec3 rotated_vertex_pos = rotation_matrix * scaled_vertex_pos;
        
        vec2 uv_position = position_to_uv(instance.position.xy, window.dims);
        vec2 vertex_uv = rotated_vertex_pos.xy / vec2(window.dims.x, window.dims.y) * 2.0;
        
        position = vec4(uv_position + vertex_uv, instance.position.z, 1.0);
        color = instance.color;
    }
    
    gl_Position = position;
    fragColor = color;
}
