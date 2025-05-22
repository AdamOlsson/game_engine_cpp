#version 450


layout(location = 0) in vec3 vertex_position;

// layout(location = 0) out vec3 uv;

void main() {
    vec3 rectangle_corners[6] = vec3[6](
        vec3(-0.5f, -0.5f, 0.0f),
        vec3(-0.5f,  0.5f, 0.0f),
        vec3( 0.5f,  0.5f, 0.0f),
        vec3( 0.5f,  0.5f, 0.0f),
        vec3( 0.5f, -0.5f, 0.0f),
        vec3(-0.5f, -0.5f, 0.0f)
    );

    vec3 corner = rectangle_corners[gl_VertexIndex];

    gl_Position = vec4(corner, 1.0);
}
