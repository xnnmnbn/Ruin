#version 450

#extension GL_EXT_nonuniform_qualifier : enable


struct Renderer2D {
    vec4  uv;
    vec4  tint;

    vec2  aberration;
    uint  layer;
    uint  texture;
    
    float brightness;
    float saturation;
    float contrast;
    float aberration_strength;
};



layout (std430, set = 0, binding = 0)
readonly buffer EntitySSBO {
    uint entities[];
};


layout (std430, set = 0, binding = 1)
readonly buffer MatrixSSBO {
    mat4 matrices[];
};

// same should be also in frag
layout (std430, set = 0, binding = 2)
readonly buffer RendererSSBO {
    Renderer2D renderers[];
};

layout (push_constant)
uniform ViewProj {
    mat4 view;
    mat4 proj;
} camera;

// same should be also in frag
layout (set = 1, binding = 0) uniform sampler2D textures[];



layout (location = 0) flat out uint out_entity_id;
layout (location = 1) out vec2 out_uv;



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
    uint e = entities[gl_InstanceIndex];
    uint tex_id = renderers[e].texture;

    vec2 vertex_pos = positions[gl_VertexIndex];

    ivec2 tex_dims = textureSize(textures[nonuniformEXT(tex_id)], 0);
    vec2  tex_size = vec2(float(tex_dims.x), float(tex_dims.y));

    gl_Position = camera.proj * camera.view * matrices[e] * vec4(vertex_pos * tex_size, 0.0, 1.0);
    

    out_entity_id = e;
    out_uv = uvs[gl_VertexIndex];
}

