#version 100
precision highp float;
precision highp sampler2D;
varying vec2 in_tex;

// Sampling
uniform sampler2D source;
uniform vec2 offset;
uniform vec2 scale;

// Post-processing
uniform float gamma;
uniform float exposure;
uniform vec4  tint;
/* 0 shows the scene as rendered, without the Reinhard curve */
uniform int   tonemap;

uniform int mode;
// 1: Halo 3's final_composite_base.hlsl curve, linear in, the 360's
// piecewise linear gamma out
uniform int tone_curve;
// RGB-defocus/chromatic aberration
// It's only uniform
uniform vec4 rgb_comp_defocus;
// Simulated gaussian blur
uniform vec2 blur_distance;

// Creates chromatic aberration
vec4 rgb_defocus()
{
    vec4 color = texture2D(source, offset + in_tex * scale).rrra;
    color.g = texture2D(source, offset + (in_tex + rgb_comp_defocus.xy) * scale).g;
    color.b = texture2D(source, offset + (in_tex + rgb_comp_defocus.zw) * scale).b;
    return color;
}

vec4 box_blur_sample()
{
    vec4 color = vec4(0);
    color += texture2D(source, offset + in_tex * scale + vec2(-blur_distance.x, blur_distance.y)) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2(-blur_distance.x, 0.0            )) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2(-blur_distance.x,-blur_distance.y)) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2(             0.0, blur_distance.y)) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2(             0.0, 0.0            )) * 0.36;
    color += texture2D(source, offset + in_tex * scale + vec2(             0.0,-blur_distance.y)) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2( blur_distance.x, blur_distance.y)) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2( blur_distance.x, 0.0            )) * 0.08;
    color += texture2D(source, offset + in_tex * scale + vec2( blur_distance.x,-blur_distance.y)) * 0.08;
    return color;
}

vec4 gaussian_blur_sample()
{
    const float sigma       = 1.0; // in taps
    const float inv_2_sigma = 1.0 / (2.0 * sigma * sigma);

    vec2  base  = offset + in_tex * scale;
    vec4  color = vec4(0.0);
    float total = 0.0;

    for(int y = -3; y <= 3; y++)
    {
        float fy = float(y);
        float wy = exp(-fy * fy * inv_2_sigma);
        for(int x = -3; x <= 3; x++)
        {
            float fx = float(x);
            float w  = wy * exp(-fx * fx * inv_2_sigma);
            color += texture2D(source, base + blur_distance * vec2(fx, fy)) * w;
            total += w;
        }
    }

    return color / total;
}

// The 360's gamma render target encoding: slopes 4, 2, 1 and 1/2
vec3 pwl_gamma(vec3 x)
{
    return mix(
        mix(x * 4.0, 0.25 + (x - 0.0625) * 2.0, step(0.0625, x)),
        mix(0.375 + (x - 0.125), 0.75 + (x - 0.5) * 0.5, step(0.5, x)),
        step(0.125, x));
}

vec4 plain_sample()
{
    return texture2D(source, offset + in_tex * scale).rgba * tint;
}

void main()
{
    vec4 color =
        mode == 2
        ? rgb_defocus()
        : mode == 1
          ? gaussian_blur_sample()
          : plain_sample();
    if(tone_curve == 1)
    {
        vec3 x    = clamp(color.rgb, vec3(0.0), vec3(1.4938016));
        color.rgb = pwl_gamma(clamp((1.0041494 - 0.15 * x * x) * x, 0.0, 1.0));
    } else
    {
        if(tonemap != 0)
            color.rgb = color.rgb / (color.rgb + vec3(1.0));
        color.rgb = pow(exposure * color.rgb, vec3(1.0 / gamma));
    }
    gl_FragColor = color;
}
