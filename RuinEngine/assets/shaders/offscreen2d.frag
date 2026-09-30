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

layout (std430, set = 0, binding = 2)
readonly buffer RendererSSBO {
    Renderer2D renderers[];
};

layout (set = 1, binding = 0) uniform sampler2D textures[];

layout (location = 0) flat in uint in_entity_id;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec4 out_color;

void main() {
    Renderer2D rend = renderers[in_entity_id];

    vec2 base_uv = rend.uv.xy + in_uv * rend.uv.zw;

    vec2 offset = rend.aberration * rend.aberration_strength;
    float r = texture(textures[nonuniformEXT(rend.texture)], base_uv + offset).r;
    vec2  gb = texture(textures[nonuniformEXT(rend.texture)], base_uv).gb;
    float b = texture(textures[nonuniformEXT(rend.texture)], base_uv - offset).b;
    float a = texture(textures[nonuniformEXT(rend.texture)], base_uv).a;

    vec3 color = vec3(r, gb.x, b);

    color *= rend.brightness;


    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = mix(vec3(luma), color, rend.saturation);

    color = (color - 0.5) * rend.contrast + 0.5;

    color *= rend.tint.rgb;
    float final_alpha = a * rend.tint.a;

    out_color = vec4(color, final_alpha);
}