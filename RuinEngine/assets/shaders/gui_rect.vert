#version 450


// anchor and size are relative percentages to screen size.
// anchor(0, 0) means center of window.
// size(1, 1) means the size of whole window.
// rotation center is relative to pivot.
// pivot(0, 0) means center of rect.

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
readonly buffer RectSSBO {
    GuiRect rects[];
};

layout (std430, set = 0, binding = 1)
readonly buffer EntitySSBO {
    uint entities[];
};



layout (push_constant) uniform Window {
    float width;
    float height;
} window_size;



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
    GuiRect rect = rects[e];

    vec2 corner = positions[gl_VertexIndex];
    vec2 window = vec2(window_size.width, window_size.height);


    vec2 local = (corner - rect.pivot) * (rect.size * window);

    float c = cos(rect.rotation);
    float s = sin(rect.rotation);
    vec2 rotated = vec2(c * local.x - s * local.y,
                        s * local.x + c * local.y);

    vec2 px  = rect.anchor * window + rotated;
    vec2 ndc = px / (window * 0.5);

    gl_Position = vec4(ndc, 0.0, 1.0);


    out_uv = mix(rect.uv.xy, rect.uv.zw, corner + 0.5);
    out_entity_id = e;
}






