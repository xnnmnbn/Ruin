#version 450

layout (location = 0) out vec2 out_uv;

const vec2 quad[6] = vec2[](
    vec2(-1.0, -1.0), // left, up
    vec2(-1.0,  1.0), // left, down
    vec2( 1.0, -1.0), // right, up

    vec2( 1.0, -1.0), // right, up
    vec2( 1.0,  1.0), // right, down
    vec2(-1.0,  1.0)  // left, down
);

const vec2 uvs[6] = vec2[](
    vec2(0.0, 0.0), // left, up
    vec2(0.0, 1.0), // left, down
    vec2(1.0, 0.0), // right, up

    vec2(1.0, 0.0), // right, up
    vec2(1.0, 1.0), // right, down
    vec2(0.0, 1.0)  // left, down
);

void main() {
    vec2 pos = quad[gl_VertexIndex];

    gl_Position = vec4(pos, 0.0, 1.0);

    out_uv = uvs[gl_VertexIndex];
}
