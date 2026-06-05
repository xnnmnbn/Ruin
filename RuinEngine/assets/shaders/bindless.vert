#version 450

#define MAX_ENTITIES 20000

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_nor;
layout (location = 2) in vec2 in_uv;

layout (location = 0) out vec2 out_uv;
layout (location = 1) flat out uint out_mat_id;

layout (set = 0, binding = 0) uniform
CameraUBO {
    mat4 view;
    mat4 proj;
} camera;

layout (std430, set = 0, binding = 1) readonly buffer
EntityBuffer {
    uint entity_ids[MAX_ENTITIES];
};

layout (std430, set = 0, binding = 2) readonly buffer
MaterialIDBuffer {
    uint material_ids[MAX_ENTITIES];
};

layout (std430, set = 0, binding = 3) readonly buffer
MatrixBuffer {
    mat4 transforms[MAX_ENTITIES];
};

void main() {
    uint entity_id = entity_ids[gl_InstanceIndex];

    mat4 model = transforms[entity_id];

    gl_Position = camera.proj * camera.view * model * vec4(in_pos, 1.0);

    out_uv = in_uv;
    out_mat_id = material_ids[entity_id];
}













