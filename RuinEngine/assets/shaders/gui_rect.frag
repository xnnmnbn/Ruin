#version 450

#extension GL_EXT_nonuniform_qualifier : enable


struct GuiRect {
    vec4  uv;
    vec4  tint;

    vec2  anchor;
    vec2  pivot;

    vec2  size;
    float rotation;
    uint  texture;

    vec4  _padding;
};

layout (std430, set = 0, binding = 0)
readonly buffer RectSBBO {
    GuiRect rects[];
};

layout (set = 1, binding = 0) uniform sampler2D textures[];

layout (location = 0) flat in uint in_entity_id;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec4 out_color;


void main() {
    GuiRect rect = rects[in_entity_id];

    vec4 color = texture(textures[nonuniformEXT(rect.texture)], in_uv);

    color *= rect.tint;

    out_color = color;
}
