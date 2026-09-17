#version 450

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_color;

layout (set = 0, binding = 0) uniform sampler2D screen_tex;

layout (push_constant)
uniform PostProcessing {
    float brightness;
    float saturation;
    float contrast;
    float invert;
    vec4  tint;
} settings;

void main() {
    vec4 screen = texture(screen_tex, in_uv);

    vec3 color = screen.rgb;

    color *= settings.brightness;

    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));

    color = mix(vec3(luma), color, settings.saturation);

    color = (color - 0.5) * settings.contrast + 0.5;

    color *= vec3(settings.tint.rgb);

    color = mix(color, 1.0 - color, settings.invert);

    out_color = vec4(clamp(color, 0.0, 1.0), screen.a);
}
