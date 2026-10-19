#version 460 core

/* Bloom blur, dual Kawase: halving (mode 0), or doubling a level up with
 * `scale` and adding `tint` times the level at the target's size (mode 1) */
layout(location=0) in vec2 f_uv;

layout(location=1, binding=0) uniform sampler2D source;
layout(location=2, binding=1) uniform sampler2D level;
layout(location=3) uniform vec2 source_texel;
layout(location=4) uniform int mode;
layout(location=5) uniform vec3 scale;
layout(location=6) uniform vec3 tint;

layout(location=0) out vec4 out_color;

void main()
{
    vec2 o = source_texel;
    vec3 sum;
    if(mode == 0)
    {
        sum = texture(source, f_uv).rgb * 4.0;
        sum += texture(source, f_uv + vec2(-o.x, -o.y)).rgb;
        sum += texture(source, f_uv + vec2(o.x, -o.y)).rgb;
        sum += texture(source, f_uv + vec2(-o.x, o.y)).rgb;
        sum += texture(source, f_uv + vec2(o.x, o.y)).rgb;
        out_color = vec4(sum / 8.0, 1.0);
        return;
    }
    sum = texture(source, f_uv + vec2(-o.x * 2.0, 0.0)).rgb;
    sum += texture(source, f_uv + vec2(-o.x, o.y)).rgb * 2.0;
    sum += texture(source, f_uv + vec2(0.0, o.y * 2.0)).rgb;
    sum += texture(source, f_uv + vec2(o.x, o.y)).rgb * 2.0;
    sum += texture(source, f_uv + vec2(o.x * 2.0, 0.0)).rgb;
    sum += texture(source, f_uv + vec2(o.x, -o.y)).rgb * 2.0;
    sum += texture(source, f_uv + vec2(0.0, -o.y * 2.0)).rgb;
    sum += texture(source, f_uv + vec2(-o.x, -o.y)).rgb * 2.0;
    out_color = vec4(scale * sum / 12.0 + tint * texture(level, f_uv).rgb, 1.0);
}
