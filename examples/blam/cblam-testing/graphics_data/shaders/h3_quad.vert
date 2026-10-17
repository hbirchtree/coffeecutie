#version 460 core

/* One triangle covering the target; uv in [0, 1] over it */
layout(location=0) uniform vec4 quad_rect;
layout(location=0) out vec2 f_uv;

void main()
{
    vec2 corner = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    f_uv        = corner * quad_rect.xy + quad_rect.zw;
    gl_Position = vec4(corner * 2.0 - 1.0, 0.0, 1.0);
}
