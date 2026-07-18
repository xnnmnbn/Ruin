#version 450

#define MAX_TEXTURES 1024
        
#extension GL_EXT_nonuniform_qualifier : enable


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
    uint entity_ids[];
};

layout(std430, set = 0, binding = 2)
readonly buffer MatrixBuffer {
    mat4 matrices[];
};

layout(std430, set = 0, binding = 3)
readonly buffer MaterialBuffer {
    Material materials[];
};


layout (set = 1, binding = 0) uniform sampler2D textures[MAX_TEXTURES];


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

    ivec2 tex_dims = textureSize(textures[nonuniformEXT(m.tex_id)], 0);
    vec2 tex_scale = vec2(float(tex_dims.x), float(tex_dims.y));

    vec2 vertex_pos = positions[gl_VertexIndex];
    vertex_pos = vertex_pos * tex_scale;
    
    out_uv          = uvs[gl_VertexIndex];

    gl_Position = proj * view * t * vec4(vertex_pos, 0.0, 1.0);

    out_tint   = m.tint;
    out_tex_id = m.tex_id;
}
