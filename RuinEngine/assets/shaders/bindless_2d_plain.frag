#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#define MAX_TEXTURES 1024

layout (location = 0) in vec4 in_tint;
layout (location = 1) in vec2 in_uv;
layout (location = 2) flat in uint in_tex_id;

layout (set = 1, binding = 0) uniform sampler2D textures[MAX_TEXTURES];

layout (location = 0) out vec4 out_color;

void main() {
    vec4 tex = texture(textures[nonuniformEXT(in_tex_id)], in_uv);

    out_color = tex * in_tint;
}



