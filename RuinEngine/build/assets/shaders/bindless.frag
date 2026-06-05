#version 450

#define MAX_ENTITIES  20000
#define MAX_TEXTURES  1024
#define MAX_MATERIALS 516


layout (location = 0) in vec2 in_uv;
layout (location = 1) flat in uint in_mat_id;

layout (location = 0) out vec4 out_color;


struct Material {
    vec4 tint;
    
    uint albedo_tex_id;
    uint normal_tex_id;
    uint metallic_tex_id;
    uint emissive_tex_id;

    float metallic;
    float roughness;
    float normal_strength;
    float emissive_strength;
};


layout (std430, set = 0, binding = 4) readonly buffer MaterialBuffer {
    Material materials[MAX_MATERIALS];
};

layout (set = 1, binding = 0) uniform sampler2D textures[MAX_TEXTURES];


void main() {
    Material material = materials[in_mat_id];

    vec4 albedo = texture(
        textures[material.albedo_tex_id],
        in_uv
    );

    out_color = albedo * material.tint;
}






