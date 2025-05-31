#version 450

layout(location = 0) in vec3 not_used;
layout(binding = 0) readonly uniform WindowDimensions {
        vec2 dims;
} window;
layout(push_constant) uniform UIElement {
    vec2 center;
    vec2 dimension;
    vec3 border_color; // Not used here
    float border_thickness; // Not used here
    float border_radius; // Not used here
} push_ui_element;

layout(location = 0) out vec2 out_ui_element_vertex;

void main() {
    const vec2 rectangle_corners[6] = vec2[](
        vec2(-0.5f, -0.5f), // Top left
        vec2(-0.5f,  0.5f), // Bottom left
        vec2( 0.5f,  0.5f), // Bottom right
        vec2( 0.5f,  0.5f), // Bottom right
        vec2( 0.5f, -0.5f), // Top right
        vec2(-0.5f, -0.5f)  // Top left
    );
    const vec2 in_vertex = rectangle_corners[gl_VertexIndex];
    
    const vec2 half_window_dimension = window.dims / 2.0;
    const vec2 vertex_position = (in_vertex * push_ui_element.dimension) + push_ui_element.center;
    const vec2 vertex_viewport_position = vertex_position / half_window_dimension;
   
    const vec2 ui_element_corner = in_vertex * push_ui_element.dimension;
    const vec2 positive_quad_offset = push_ui_element.dimension / 2.0;

    // Let the GPU interpolate between (0,0) and (push_ui_element.dimension)
    out_ui_element_vertex = ui_element_corner + positive_quad_offset;



    gl_Position = vec4(vertex_viewport_position, 0.0, 1.0);
}
