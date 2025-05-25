#version 450

// ##### From the cpu #####
#define window_dimension vec2(800.0, 800.0)
#define ui_element_center vec2(0.0, 300.0) // Window coordinates
#define ui_element_dimension vec2(800.0, 200.0) // Pixel size
#define ui_element_border_thickness 20.0 // Pixel size 
#define ui_element_border_radius 30.0 // Pixel size
// ########################

layout(location = 0) in vec3 not_used;
layout(location = 0) out vec2 out_ui_element_vertex;

// #### Only here for development, in the fragment shader these values will be read
// #### from the cpu, like push-constants
layout(location = 1) out float out_ui_element_border_thickness;
layout(location = 2) out float out_ui_element_border_radius;
layout(location = 3) out vec2 out_ui_element_dimension;
// ####

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
    
    const vec2 half_window_dimension = window_dimension / 2.0;
    const vec2 vertex_position = (in_vertex * ui_element_dimension) + ui_element_center;
    const vec2 vertex_viewport_position = vertex_position / half_window_dimension;
   
    const vec2 ui_element_corner = in_vertex * ui_element_dimension;
    const vec2 positive_quad_offset = ui_element_dimension / 2.0;

    // Let the GPU interpolate between (0,0) and (ui_element_dimension)
    out_ui_element_vertex = ui_element_corner + positive_quad_offset;

    out_ui_element_border_thickness = ui_element_border_thickness;
    out_ui_element_border_radius = ui_element_border_radius;
    out_ui_element_dimension = ui_element_dimension;


    gl_Position = vec4(vertex_viewport_position, 0.0, 1.0);
}
