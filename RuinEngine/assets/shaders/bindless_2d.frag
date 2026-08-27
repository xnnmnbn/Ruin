#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#define MAX_TEXTURES 1024


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


layout (location = 0) in vec2 in_uv;
layout (location = 1) flat in uint in_entity;


layout (std430, set = 0, binding = 3)
readonly buffer MaterialBuffer {
    Material2D materials[];
};

layout (set = 1, binding = 0) uniform sampler2D textures[MAX_TEXTURES];



layout (location = 0) out vec4 out_color;


float pseudo_noise(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void main() {
    Material2D m = materials[in_entity];

    vec2 uv = in_uv;
    
    if (m.glitch > 0.0) {
        float slice = floor(in_uv.y * 20.0);
        float noise = pseudo_noise(vec2(slice, m.glitch));

        if (noise < m.glitch) {
            uv.x += (pseudo_noise(vec2(uv.y, noise)) - 0.5) * m.glitch * 0.1;
        }
    }

    vec3 color;
    if (m.chromatic_aberration > 0.0) {
        vec2 offset = vec2(m.chromatic_aberration * 0.02, 0.0);
        float r = texture(textures[nonuniformEXT(m.tex_id)], uv + offset).r;
        float g = texture(textures[nonuniformEXT(m.tex_id)], uv).g;
        float b = texture(textures[nonuniformEXT(m.tex_id)], uv - offset).b;
        color = vec3(r, g, b);
    } else {
        color = texture(textures[nonuniformEXT(m.tex_id)], uv).rgb;
    }

    float alpha = texture(textures[nonuniformEXT(m.tex_id)], uv).a;

    color += m.brightness;

    color = (color - 0.5) * m.contrast + 0.5;

    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = mix(vec3(luma), color, m.saturation);

    color = mix(color, vec3(1.0) - color, m.inverse);

    if (m.bloom < 1.0) {
        float current_luma = dot(color, vec3(0.299, 0.587, 0.114));
        if (current_luma > m.bloom) {
            color += color * (current_luma - m.bloom) * 2.0;
        }
    }

    color *= m.tint.rgb;
    alpha *= m.tint.a;

    
    out_color = vec4(clamp(color, 0.0, 1.0), alpha);
}



