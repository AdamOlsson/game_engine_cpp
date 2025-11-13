#version 450

#define CIRCLE 1
#define TRIANGLE 2
#define RECTANGLE 3

layout(location = 0) in vec2 in_local_position;  // -0.5 to 0.5
layout(location = 1) flat in vec4 in_color;
layout(location = 2) flat in int in_geometry_type;
layout(location = 3) flat in float in_border_width_px;
layout(location = 4) flat in float in_border_radius_px;
layout(location = 5) flat in vec4 in_border_color;

layout(location = 0) out vec4 out_color;

float signed_distance_box(vec2 point, vec2 box_dimensions) {
    const vec2 q = abs(point) - (box_dimensions * 0.5);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0));
}

void main() {
    if(in_geometry_type == RECTANGLE) {
        const vec2 box_dims_px = vec2(24,24); // TODO: Can we get this from the view projection matrix?
        const vec2 border_width_px = vec2(in_border_width_px);
        const vec2 position_px = in_local_position * box_dims_px;

        const float distance_outer_edge = signed_distance_box(position_px, box_dims_px);
        const float distance_inner_edge = signed_distance_box(position_px, box_dims_px - border_width_px);

        const bool inside_outer_edge = distance_outer_edge <= 0.0;
        const bool outside_inner_edge = distance_inner_edge > 0.0;
        const bool is_on_border = inside_outer_edge && outside_inner_edge;

        if (is_on_border) {
            out_color = in_border_color;
        } else if(inside_outer_edge) {
            out_color = in_color;
        } else {
            discard;
        }
    }
}

