#version 320 es

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;

in FragData {
    vec3 world_pos;
    vec2 tex;
    vec2 light_tex;
    flat int instanceId;
} frag;

uniform float render_distance;

out vec4 out_color;

void main()
{
    float dist = length(frag.world_pos - vec3(0.0));

    if(dist < (render_distance - 10.0))
        discard;

    float fade_in = abs((dist - render_distance + 10.0) / 10.0);
    float fade_out = 1.0 - abs((dist - render_distance) / 10.0);

    out_color = vec4(0.0, 0.2, 1.0, fade_out);
}
