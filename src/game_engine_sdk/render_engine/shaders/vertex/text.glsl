#version 450

struct CharacterData {
    vec2 position;
    vec4 uvwt; // bbox for texture
    uint text_segment_idx;
};

struct TextSegmentData {
    vec4 font_color;
    uint font_size;
    float font_rotation;
    float font_weight;
};

layout(std140, binding = 0) readonly buffer CharacterDataBlock {
    CharacterData instances[1024];
} character_data_block;

layout(std140, binding = 3) readonly buffer TextSegmentDataBlock {
    TextSegmentData instances[16];
} text_segment_data_block;

layout(binding = 1) readonly uniform WindowDimensions {
        vec2 dims;
} window;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec4 out_frag_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out float out_font_weight;

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
    CharacterData character_data = character_data_block.instances[gl_InstanceIndex];
    TextSegmentData text_data = text_segment_data_block.instances[character_data.text_segment_idx];
    vec4 font_color = text_data.font_color;
    uint font_size = text_data.font_size;
    float font_weight = text_data.font_weight;
    float rotation = text_data.font_rotation;


    vec3 scaled_vertex_pos = scale_vertex(in_position, font_size, font_size);
    mat3 rotation_matrix = rotationMatrixZ(-rotation);
    vec3 rotated_vertex_pos = rotation_matrix * scaled_vertex_pos;

    vec2 viewport_position = positions_to_viewport(character_data.position.xy, window.dims);
    vec2 vertex_in_viewport = rotated_vertex_pos.xy / vec2(window.dims.x, window.dims.y) * 2.0;
    vec4 position = vec4(viewport_position + vertex_in_viewport, 0.0, 1.0);
    
    gl_Position = position;
    out_frag_color = font_color;
    out_uv = compute_uv(in_position.xy, character_data.uvwt);
    out_font_weight = font_weight;

}
