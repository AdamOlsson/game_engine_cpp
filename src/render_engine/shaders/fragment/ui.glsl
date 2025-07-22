#version 450

layout(location = 0) in vec2 in_fragment;
layout(push_constant) uniform UIElement {
    vec2 center; // Not used here
    vec2 dimension;
    vec4 background_color;
    vec4 border_color;
    float border_thickness;
    float border_radius;
} push_ui_element;

layout(location = 0) out vec4 out_color;

// ################################
// ##### Supporting functions #####
// ################################
vec4 calculate_inner_border_color(
    vec2 fragment_position, vec2 corner_center, float border_radius,
    float border_thickness, vec4 border_color, vec4 background_color);
bool is_on_border(
    const vec2 abs_fragment_position, const vec2 ui_element_dimension,
    const float ui_element_border_thickness);

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

    if (is_corner) {
      out_color = calculate_inner_border_color(
                    abs_fragment_position,
                    rounded_corner_center_position,
                    push_ui_element.border_radius,
                    push_ui_element.border_thickness,
                    push_ui_element.border_color,
                    push_ui_element.background_color);
    } else if(is_on_border(abs_fragment_position, 
                            push_ui_element.dimension,
                            push_ui_element.border_thickness)) {
        float alpha = 1.0;
        out_color = vec4(push_ui_element.border_color.rgb, alpha);
    } else {
        // background
        out_color = push_ui_element.background_color;
    }
}

vec4 calculate_inner_border_color(
    vec2 fragment_position,
    vec2 corner_center,
    float border_radius,
    float border_thickness,
    vec4 border_color,
    vec4 background_color
) {
    float distance_to_center = distance(fragment_position, corner_center);
    float outer_radius = border_radius;
    float inner_radius = border_radius - border_thickness;
    
    // Calculate how much this fragment is covered by the shape
    float outer_coverage = 1.0 - smoothstep(outer_radius - 0.5, outer_radius + 0.5, distance_to_center);
    float inner_coverage = 1.0 - smoothstep(inner_radius - 0.5, inner_radius + 0.5, distance_to_center);
    float border_coverage = outer_coverage - inner_coverage;
    
    vec3 final_color = mix(background_color.rgb, border_color.rgb, border_coverage);
    float final_alpha = max(outer_coverage * border_color.a, inner_coverage * background_color.a);
    
    return vec4(final_color, final_alpha);
}



bool is_on_border(
    const vec2 abs_fragment_position, const vec2 ui_element_dimension, const float ui_element_border_thickness
){
    // Half ui_element_dimension because the abs_fragment_position is centered to origo
    const vec2 border_threshold = (ui_element_dimension / 2.0) - ui_element_border_thickness;
    const bool x_larger_than_border_thresh = abs_fragment_position.x > border_threshold.x;
    const bool y_larger_than_border_thresh = abs_fragment_position.y > border_threshold.y;
    return x_larger_than_border_thresh || y_larger_than_border_thresh;
}


