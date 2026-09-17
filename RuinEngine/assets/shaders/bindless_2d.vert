#version 450

#define MAX_TEXTURES 1024
        
#extension GL_EXT_nonuniform_qualifier : enable

struct Material2D {
    uint  tex_id;
    float brightness;
    float saturation;
    float contrast;
    
    float inverse;
    float bloom;
    float glitch;
    float chromatic_aberration;
    
    vec4  tint;
};

layout (location = 0) out vec2 out_uv;
layout (location = 1) flat out uint out_entity;


layout (std140, set = 0, binding = 0)
uniform CameraBuffer {
    mat4 proj;
    mat4 view;
};

layout (std430, set = 0, binding = 1)
readonly buffer EntityBuffer {
    uint entity_ids[];
};

layout (std430, set = 0, binding = 2)
readonly buffer MatrixBuffer {
    mat4 matrices[];
};

layout (std430, set = 0, binding = 3)
readonly buffer MaterialBuffer {
    Material2D materials[];
};


layout (set = 1, binding = 0) uniform sampler2D textures[MAX_TEXTURES];


const vec2 positions[6] = vec2[](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2(-0.5,  0.5),

    vec2(-0.5,  0.5),
    vec2( 0.5, -0.5),
    vec2( 0.5,  0.5)
);

const vec2 uvs[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),

    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);

void main() {
    uint       e = entity_ids[gl_InstanceIndex];
    Material2D m = materials[e];
    mat4       t = matrices[e];

    ivec2 tex_dims = textureSize(textures[nonuniformEXT(m.tex_id)], 0);
    vec2 tex_scale = vec2(float(tex_dims.x), float(tex_dims.y));

    vec2 vertex_pos = positions[gl_VertexIndex];
    vertex_pos      = vertex_pos * tex_scale;
    
    out_uv     = uvs[gl_VertexIndex];
    out_entity = e;

    gl_Position = proj * view * t * vec4(vertex_pos, 0.0, 1.0);
}
