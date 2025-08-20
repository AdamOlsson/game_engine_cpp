#version 450

#define CIRCLE 0
#define RECTANGLE 1
#define POLYGON 2


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_frag_color;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_border_color;
layout(location = 4) in float in_border_thickness;
layout(location = 5) in float in_border_radius;

layout(binding = 1) readonly uniform WindowDimensions {
        vec2 dims;
} window;

layout(binding = 2) uniform sampler2D texture_sampler;

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
    vec2 ap = point - a;
    vec2 ab = b - a;
    float h = clamp(dot(ap, ab) / dot(ab, ab), 0.0, 1.0);
    return length(ap - ab * h);
}

float signed_distance_polygon(
    vec3 point, const vec3 vertices[MAX_VERTICES], const int num_vertices
) {
    // Rounder corners for arbitrary polygons is quite complex, therefore I do
    // not support it
    float distance = 1e9;
    float winding = 0.0;

    for(int i = 0; i < num_vertices; i++){
        vec2 a  = vertices[i].xy;
        vec2 b  = vertices[(i + 1) % num_vertices].xy;
        
        distance = min(distance, unsigned_distance_to_segment(point.xy, a, b));
        
        // Accumulate winding (signed angle)
        vec2 pa = a - point.xy;
        vec2 pb = b - point.xy;
        float cross = pa.x * pb.y - pa.y * pb.x;
        float dot_product = dot(pa, pb);
        winding += atan(cross, dot_product);
    }
    
    float sign = (abs(winding) > 1e-3) ? -1.0 : 1.0;

    return distance * sign; // negative inside, positive outside
}

float signed_distance_circle(vec3 point, vec3 center, float radius) {
    return length(point - center) - radius; // negative inside
}

float signed_distance_rectangle(vec3 point, float radius) {
    const vec2 half_size = vec2(0.5, 0.5);
    vec2 p = point.xy;
    vec2 q = abs(p) - (half_size - vec2(radius));

    float outside = length(max(q, 0.0)) - radius;
    float inside  = min(max(q.x, q.y), 0.0);
    return outside + inside;
}

float signed_distance(vec3 point) {
    if(vertices.shape == CIRCLE) {
        // For circles, we do not use the vertices
        vec3 center = vec3(0.0);
        float radius = 0.5;
        return signed_distance_circle(point, center, radius);
    } else if(vertices.shape == RECTANGLE) {
        // For rectangles, we do not use the vertices
        return signed_distance_rectangle(point, in_border_radius);
    } else if(vertices.shape == POLYGON) {
        return signed_distance_polygon(point, vertices.vertices, vertices.num_vertices);
    }
}

void main() {
    float distance = signed_distance(in_position);
    float aa = fwidth(distance);
    float shape_mask = 1.0 - smoothstep(-aa, aa, distance);

    float border_inner_edge = -in_border_thickness;
    float border_mask = shape_mask * smoothstep(border_inner_edge - aa, border_inner_edge + aa, distance);

    vec4 color = mix(in_frag_color, in_border_color, border_mask);
    color.a *= shape_mask;
    
    if (color.a <= 0.001) discard;
    
    out_color = color;
}
