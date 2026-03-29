#version 450


struct GlyphSBO {
    uint text_id; 
    vec3 offset;
};

struct TextSBO {
    vec3 position;
    vec4 color;
    float font_size;
};

layout(binding = 0) readonly buffer TextStorageBuffer {
    TextSBO instances[]; 
} text_buffer;


layout(binding = 1) readonly buffer GlyphStorageBuffer {
    GlyphSBO instances[]; 
} glyph_buffer;

// layout(binding = 1) readonly uniform TextUniformBuffer {
// } uniform_buffer;

layout(push_constant) uniform CameraMatrix {
    mat4 view_projection;
} pc_camera;

layout(location = 0) in vec3 in_world_position;
layout(location = 1) in vec3 in_uvw;

layout(location = 0) out int out_is_interior;
layout(location = 1) out vec3 out_uvw;
layout(location = 2) out float out_winding_order;
layout(location = 3) out vec4 out_color;

mat4 translate(vec3 position) {
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        position.x, position.y, position.z, 1.0
    );
}

mat4 scale(vec3 s) {
    return mat4(
        s.x, 0.0, 0.0, 0.0,
        0.0, s.y, 0.0, 0.0,
        0.0, 0.0, s.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

int is_interior(vec3 uvw) {
    return uvw.x == 0.0 && uvw.y == 0.0 && uvw.z == 0.0 ? 1 : 0;
}

void main() {
    GlyphSBO glyph_instance = glyph_buffer.instances[gl_InstanceIndex];
    TextSBO text_instance = text_buffer.instances[glyph_instance.text_id];

    float font_size = text_instance.font_size;

    mat4 text_model_matrix = translate(text_instance.position);

    mat4 glyph_model_matrix = translate(glyph_instance.offset) *scale(vec3(font_size, font_size, 1.0f));


    mat4 model_matrix = text_model_matrix * glyph_model_matrix;

    gl_Position = pc_camera.view_projection * model_matrix * vec4(in_world_position.xy, 0.0, 1.0);
    out_is_interior = is_interior(in_uvw);
    out_uvw = in_uvw;
    out_winding_order = in_world_position.z;
    out_color = text_instance.color;
}
