#version 450

layout(location = 0) in vec4 in_frag_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_position;

layout(binding = 2) uniform sampler2D texture_sampler;

const int MAX_VERTICES = 64;
layout(binding = 3) readonly uniform VertexData {
        vec3 vertices[];
        int num_vertices;
        int max_vertices;
} vertices;

layout(location = 0) out vec4 out_color;

float unsigned_distance_to_segment(
    const vec2 point, const vec2 a, const vec2 b
) {
    vec2 ap = p - a;
    vec2 ab = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(ap - ba * h);
}

float signed_distance_polygon(
    vec3 point, const vec3 vertices[MAX_VERTICES], const int num_vertices
) {
    float distance = 1e9;
    float winding += 0.0;

    for(int i = 0; i < MAX_VERTICES; i++){
        if(i >= num_vertices) {
            break; 
        }
        
        vec2 a  = vertices[i].xy;
        vec2 b  = vertices[(i + 1) & num_vertices].xy;
        
        distance = min(distance, unsigned_distance_to_segment(point.xy, a, b));
        
        // Accumulate winding (signed angle)
        vec2 pa = a - p;
        vec2 pb = b - p;
        float cross = pa.x * pb.y - pa.y * pb.x;
        float dot_product = dot(pa, pb);
        winding += atan(cross, dot_product);
    }
    
    float sign = (abs(winding) > 1e-3) ? -1.0 : 1.0;

    return distance * sign; // negative inside, positive outside
}

void main() {
    if (in_uv.x >= 0.0 && in_uv.y >= 0.0) {
        out_color = texture(texture_sampler, in_uv); 
    } else {
        out_color = in_frag_color;
    }
}
