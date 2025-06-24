#version 450

layout(location = 0) in vec2 in_fragment;
layout(push_constant) uniform UIElement {
    vec2 center; // Not used here
    vec2 dimension;
    vec3 border_color;
    float border_thickness;
    float border_radius;
} push_ui_element;

layout(location = 0) out vec4 out_color;

// ################################
// ##### Supporting functions #####
// ################################
float determine_alpha_for_corner(
    const vec2 abs_fragment_position, const vec2 ui_element_rounded_corner_center, 
    const float ui_element_border_thickness, const float ui_element_border_radius
);

float determine_alpha_for_edge(
    const vec2 abs_fragment_position, const vec2 ui_element_dimension, const float ui_element_border_thickness
);

// ################################
// ############# Main #############
// ################################
void main() {
    const vec2 origo_centered_vertex = in_fragment - (push_ui_element.dimension / 2.0);
    const vec2 abs_fragment_position = abs(origo_centered_vertex);
   
    // corner of a origo centered ui element
    const vec2 ui_element_corner = push_ui_element.dimension / 2.0;
    const vec2 rounded_corner_center_position =  ui_element_corner - push_ui_element.border_radius;
    bool is_corner = 
        abs_fragment_position.x > rounded_corner_center_position.x &&
        abs_fragment_position.y > rounded_corner_center_position.y;

    float alpha = 0.0;
    if (is_corner) {
        alpha = determine_alpha_for_corner(
            abs_fragment_position, rounded_corner_center_position, push_ui_element.border_thickness, push_ui_element.border_radius);
    } else {
        alpha = determine_alpha_for_edge(
            abs_fragment_position, push_ui_element.dimension, push_ui_element.border_thickness);
    }
    out_color = vec4(push_ui_element.border_color, alpha);
}

float determine_alpha_for_corner(
    const vec2 abs_fragment_position, const vec2 ui_element_rounded_corner_center, 
    const float ui_element_border_thickness, const float ui_element_border_radius
) {
    const float rounded_corner_center_distance = 
        distance(abs_fragment_position, ui_element_rounded_corner_center);

    const float inner_threshold =
        ui_element_border_radius - ui_element_border_thickness;
    const bool larger_than_inner_border_radius =
        rounded_corner_center_distance > inner_threshold;
    const bool less_than_outer_border_radious =
        rounded_corner_center_distance < ui_element_border_radius;

    return float(larger_than_inner_border_radius && less_than_outer_border_radious);
}

float determine_alpha_for_edge(
    const vec2 abs_fragment_position, const vec2 ui_element_dimension, const float ui_element_border_thickness
){
    // Half ui_element_dimension because the abs_fragment_position is centered to origo
    const vec2 border_threshold = (ui_element_dimension / 2.0) - ui_element_border_thickness;
    const bool x_larger_than_border_thresh = abs_fragment_position.x > border_threshold.x;
    const bool y_larger_than_border_thresh = abs_fragment_position.y > border_threshold.y;
    return float(x_larger_than_border_thresh || y_larger_than_border_thresh);
}
