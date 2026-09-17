#version 450
    
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) flat out uint out_mat_idx;
layout (location = 1) out vec2 out_uv;

struct GuiRect {
    uint  material;
    int   _layer;
    float rotation;
    uint  _padding;

    vec2  position;
    vec2  scale;
    vec2  pivot;
};





layout (std430, set = 0, binding = 0)
readonly buffer GuiBuffer {
    GuiRect gui_rects[];
};

layout (std140, set = 0, binding = 1)
readonly buffer GuiIndexBuffer {
    uint gui_indices[];
};

layout (push_constant)
uniform ScreenSize {
    uint width;
    uint height;
} screen_size;





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
    uint gui_idx = gui_indices[gl_InstanceIndex];
    GuiRect rect = gui_rects[gui_idx];
    
    rect.scale.y = -rect.scale.y;
    rect.position.y = screen_size.height - rect.position.y;


    vec2 local_pos = positions[gl_VertexIndex];
    vec2 centered_uv = local_pos + vec2(0.5);
    vec2 scaled_pos = (centered_uv - rect.pivot) * rect.scale;

    float c = cos(rect.rotation);
    float s = sin(rect.rotation);
    vec2 rotated_pos = vec2(
        scaled_pos.x * c - scaled_pos.y * s,
        scaled_pos.x * s + scaled_pos.y * c
    );

    vec2 world_pixel_pos = rotated_pos + rect.position;

    vec2 screen_dim = vec2(float(screen_size.width), float(screen_size.height));
    
    vec2 ndc_pos = (world_pixel_pos / screen_dim) * 2.0 - 1.0;
    ndc_pos.y = -ndc_pos.y;

    gl_Position = vec4(ndc_pos, 0.0, 1.0);

    out_mat_idx = rect.material;
    out_uv = uvs[gl_VertexIndex];
}






