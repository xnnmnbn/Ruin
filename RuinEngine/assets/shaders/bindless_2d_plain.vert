#version 450

#define MAX_ENTITIES 20000

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec4 out_tint;
layout (location = 1) out vec2 out_uv;
layout (location = 2) flat out uint out_tex_id;

struct EntityData {
    mat4 matrix;
    vec4 tint;
    uint tex_id;
};

layout (set = 0, binding = 0) uniform
CameraUniform {
    mat4 view;
    mat4 proj;
} camera;

layout (std430, set = 0, binding = 1) readonly buffer
EntityBuffer {
    uint entity_ids[MAX_ENTITIES];
};

layout (std430, set = 0, binding = 2) readonly buffer
EntityDataBuffer {
    EntityData datas[MAX_ENTITIES];
};



void main() {
    uint entity_id = entity_ids[gl_InstanceIndex];

    EntityData data = datas[entity_id];

    gl_Position = camera.proj * camera.view * data.matrix * vec4(in_pos, 1.0);

    out_tint = data.tint;
    out_uv = in_uv;
    out_tex_id = data.tex_id;
}
