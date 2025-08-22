#version 450

#define CIRCLE 0
#define RECTANGLE 1
#define POLYGON 2

layout(location = 0) in vec3 in_position_px;
layout(location = 1) in vec4 in_frag_color;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_border_color;
layout(location = 4) in float in_border_thickness_px;
layout(location = 5) in float in_border_radius_px;
layout(location = 6) in vec2 in_shape_dimension_px;

layout(binding = 1) readonly uniform WindowDimensions {
        vec2 dims;
} window;

layout(binding = 2) uniform sampler2D u_texture_sampler;

const int MAX_VERTICES = 64;
layout(binding = 3) readonly uniform VertexData {
        vec3 vertices[MAX_VERTICES];
        int num_vertices;
        int shape;
} vertices;

layout(location = 0) out vec4 out_color;


float unsigned_distance_to_segment(
    const vec2 point, const vec2 a, const vec2 b
) {
    const vec2 ap = point - a;
    const vec2 ab = b - a;
    const float h = clamp(dot(ap, ab) / dot(ab, ab), 0.0, 1.0);
    return length(ap - ab * h);
}

float signed_distance_polygon(
    const vec2 point, const vec3 vertices[MAX_VERTICES], const int num_vertices,
    const vec2 shape_aspect
) {
    // Rounder corners for arbitrary polygons is quite complex, therefore I do
    // not support it
    float distance = 1e9;
    float winding = 0.0;

    for(int i = 0; i < num_vertices; i++){
        const vec2 a  = vertices[i].xy*shape_aspect;
        const vec2 b  = vertices[(i + 1) % num_vertices].xy*shape_aspect;
        
        distance = min(distance, unsigned_distance_to_segment(point.xy, a, b));
        
        // Accumulate winding (signed angle)
        const vec2 pa = a - point.xy;
        const vec2 pb = b - point.xy;
        const float cross = pa.x * pb.y - pa.y * pb.x;
        const float dot_product = dot(pa, pb);
        winding += atan(cross, dot_product);
    }
    
    const float sign = (abs(winding) > 1e-3) ? -1.0 : 1.0;

    return distance * sign; // negative inside, positive outside
}

float signed_distance_circle(const vec2 point, const vec2 center, const float radius) {
    return length(point - center) - radius; // negative inside
}

float signed_distance_rectangle(const vec2 point, const float radius, const vec2 shape_aspect) {
    const vec2 half_size = shape_aspect / 2.0;
    const vec2 q = abs(point.xy) - (half_size - vec2(radius));
    const float outside = length(max(q, 0.0)) - radius;
    const float inside  = min(max(q.x, q.y), 0.0);
    return outside + inside;
}

float signed_distance(const vec3 point) {
    if(vertices.shape == CIRCLE) {
        // For circles, we do not use the vertices
        const vec2 center = vec2(0.0);
        const float radius = in_shape_dimension_px.x / 2.0;
        return signed_distance_circle(point.xy, center, radius);
    } else if(vertices.shape == RECTANGLE) {
        // For rectangles, we do not use the vertices
        return signed_distance_rectangle(point.xy, in_border_radius_px, in_shape_dimension_px);
    } else if(vertices.shape == POLYGON) {
        return signed_distance_polygon(point.xy, vertices.vertices, vertices.num_vertices, in_shape_dimension_px);
    }
}


float border_sdf(
    const vec2 point, const float radius, const float thickness, const vec2 shape_aspect 
){
    // Outer rounded rectangle
    float d_outer = signed_distance_rectangle(point, radius, shape_aspect);

    // Inner rounded rectangle (shrunk by thickness, smaller radius)
    float inner_radius = max(radius - thickness, 0.0);
    vec2 inner_dim = shape_aspect - 2.0 * vec2(thickness);
    float d_inner = signed_distance_rectangle(point, inner_radius, inner_dim);

    // Ring = outside outer OR inside inner
    return max(d_outer, -d_inner);
}

void main() {
    const float distance_px = signed_distance(in_position_px);
    const float aa = fwidth(distance_px);
    const float shape_mask = 1.0 - smoothstep(-aa, aa, distance_px);

   
   float border_mask = 0.0;
    if (vertices.shape == RECTANGLE) {
        // Compute inner rounded borders, only applicable for rectangles.
        // Rounded borders for any other shape is not supported due to complexity.
        const float border_distance_px = border_sdf(in_position_px.xy, in_border_radius_px, in_border_thickness_px, in_shape_dimension_px);
        border_mask = 1.0 - smoothstep(-aa, aa, border_distance_px);
    } else {
        const float border_inner_edge = -in_border_thickness_px;
        border_mask = shape_mask * smoothstep(border_inner_edge - aa, border_inner_edge + aa, distance_px);
    }
   
    vec4 shape_color = in_frag_color;
    if(in_uv.x >= 0.0 && in_uv.y >= 0.0) {
        shape_color = texture(u_texture_sampler, in_uv); 
    }

    vec4 color = mix(shape_color, in_border_color, border_mask);
    color.a *= shape_mask;
    
    if (color.a <= 0.001) discard;
    
    out_color = color;
}
