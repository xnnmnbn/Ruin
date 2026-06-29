#version 450

layout(location = 0) out vec4 out_tint;
layout(location = 1) out vec2 out_uv;
layout(location = 2) flat out uint out_tex_id;

struct Material {
    vec4 tint;
    uint tex_id;
};

layout(std140, set = 0, binding = 0)
uniform CameraBuffer {
    mat4 proj;
    mat4 view;
};

layout(std430, set = 0, binding = 1)
readonly buffer EntityBuffer {
    // Acquire by gl_InstanceIndex;
    uint entity_ids[];
};

layout(std430, set = 0, binding = 2)
readonly buffer MatrixBuffer {
    // Acquire by entity id.
    mat4 matrices[];
};

layout(std430, set = 0, binding = 3)
readonly buffer MaterialBuffer {
    // Acquire by entity id.
    Material materials[];
};

// Procedural positioning arrays for 2 Triangles forming a full Quad
const vec2 positions[6] = vec2[](
    vec2(-0.5, -0.5), // Top Left
    vec2( 0.5, -0.5), // Top Right
    vec2(-0.5,  0.5), // Bottom Left

    vec2(-0.5,  0.5), // Bottom Left
    vec2( 0.5, -0.5), // Top Right
    vec2( 0.5,  0.5)  // Bottom Right
);

// Standard 0.0 to 1.0 UV mappings matching the positional layout
const vec2 uvs[6] = vec2[](
    vec2(0.0, 0.0), // Top Left
    vec2(1.0, 0.0), // Top Right
    vec2(0.0, 1.0), // Bottom Left

    vec2(0.0, 1.0), // Bottom Left
    vec2(1.0, 0.0), // Top Right
    vec2(1.0, 1.0)  // Bottom Right
);

void main() {
    uint e = entity_ids[gl_InstanceIndex];

    Material m = materials[e];

    mat4 t = matrices[e];

    // Grab current vertex properties based on gl_VertexIndex (0 through 5)
    vec2 vertex_pos = positions[gl_VertexIndex];
    out_uv          = uvs[gl_VertexIndex];

    // Vulkan Clip Space Calculation
    gl_Position = proj * view * t * vec4(vertex_pos, 0.0, 1.0);

    // Forward descriptors to Fragment stage
    out_tint   = m.tint;
    out_tex_id = m.tex_id;
}
