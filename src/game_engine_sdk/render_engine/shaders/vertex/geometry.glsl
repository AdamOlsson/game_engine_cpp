#version 450

#define PI 3.14159265
#define CIRCLE 1
#define TRIANGLE 2
#define RECTANGLE 3
#define HEXAGON 4
#define ARROW 5 
#define LINE 6 

struct BorderData {
    vec4 color;
    float thickness;
    float radius;
};

struct InstanceData {
    vec3 center;
    vec2 dimension;
    float rotation;
    uint texture_idx;
    vec4 color;
    vec4 uvwt; 
    BorderData border;
};

layout(std140, binding = 0) readonly buffer InstanceDataBlock {
    InstanceData instances[1024];
} instance_data_block;

layout(binding = 1) readonly uniform WindowDimensions {
        vec2 dims;
} window;

layout(push_constant) uniform CameraMatrix {
    mat4 view_projection;
} pc_camera;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_position_px;
layout(location = 1) out vec4 out_frag_color;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_border_color;
layout(location = 4) out float out_border_thickness_px;
layout(location = 5) out float out_border_radius_px;
layout(location = 6) out vec2 out_shape_dimension_px;
layout(location = 7) out uint out_texture_idx;

mat3 create_rotation_matrix_z(float theta) {
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

vec2 positions_to_viewport(vec2 pixel_pos, vec2 window_dims) {
    return vec2(
        (2.0 * pixel_pos.x) / window_dims.x,
        -(2.0 * pixel_pos.y) / window_dims.y
    );
}

/**
 * @brief Compute the uv coordinates.
 * 
 * Compute the uv coordinates using the raw vertex position and the bounding box for the
 * texture.
 *
 * @param vertex The raw vertex position. All values span between -0.5f and 0.5f.
 * @param bbox The bounding box of texture where uv is the top left and wt the bottom
 *             right. The values span between 0.0f and 1.0f.
 * @return vec2 The uv coordinate for where to sample the texture. If any of the bbox
 *              values are below 0 the bbox is invalid and we return an invalid uv.
 */
vec2 compute_uv(vec2 vertex, vec4 bbox) {
    if(bbox.x < 0 || bbox.y < 0 || bbox.z < 0 || bbox.w < 0) {
        return vec2(-1.0f, -1.0f);
    }

    vec2 bbox_dimension = bbox.zw - bbox.xy;
    vec2 bbox_offset = bbox.xy;
    // Offset the vertex to span between 0.0f and 1.0f
    vec2 vertex_offset = vertex + vec2(0.5f, 0.5f);
    return vertex_offset * bbox_dimension + bbox_offset; 
}

void main() {
    InstanceData instance = instance_data_block.instances[gl_InstanceIndex];
    
    // To world space
    vec3 in_position_px = scale_vertex(in_position, instance.dimension.x, instance.dimension.y);
    
    // Rotate in world space around origo
    mat3 rotation_matrix = create_rotation_matrix_z(-instance.rotation);
    in_position_px = rotation_matrix * in_position_px;
    
    // To view space
    vec2 viewport_position = positions_to_viewport(instance.center.xy, window.dims);
    vec2 vertex_in_viewport = in_position_px.xy / vec2(window.dims.x, window.dims.y) * 2.0;
    
    gl_Position = vec4(viewport_position + vertex_in_viewport, instance.center.z, 1.0);
    out_position_px = vec3(in_position.xy * instance.dimension, 0.0);
    out_frag_color = instance.color;
    out_uv = compute_uv(in_position.xy, instance.uvwt);
    out_border_color = instance.border.color;
    out_border_thickness_px = instance.border.thickness;
    out_border_radius_px = instance.border.radius;
    out_shape_dimension_px = instance.dimension; 
    out_texture_idx = instance.texture_idx;
}
