#version 450

layout(location = 0) in vec3 in_vertex_;
layout(binding = 0) readonly uniform WindowDimensions {
        vec2 dims;
} window;
layout(push_constant) uniform UIElement {
    vec2 center;
    vec2 dimension;
    vec4 background_color; // Not used here
    vec4 border_color; // Not used here
    float border_thickness; // Not used here
    float border_radius; // Not used here
} push_ui_element;

layout(location = 0) out vec2 out_ui_element_vertex;

vec2 position_to_viewport(vec2 vertex_pos, vec2 window_dims) {
    return vec2(
        (2.0 * vertex_pos.x) / window_dims.x,
        -(2.0 * vertex_pos.y) / window_dims.y
    );
}

void main() {
    const vec2 in_vertex = in_vertex_.xy;
    
    const vec2 viewport_position = position_to_viewport(push_ui_element.center, window.dims);

    const vec2 ui_element_corner = in_vertex * push_ui_element.dimension;
    const vec2 vertex_offset_viewport = (ui_element_corner * 2.0) / window.dims;
    
    out_ui_element_vertex = ui_element_corner;
    gl_Position = vec4(viewport_position + vertex_offset_viewport, 0.0, 1.0);
}
