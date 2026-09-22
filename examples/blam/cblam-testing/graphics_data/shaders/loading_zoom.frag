#version 100

// Radial zoom blur standing in for the original's accumulation buffer: the
// Xbox built the loading image by adding ~15 progressively scaled copies of
// the frame, which is why it has exactly 16 quantisation levels. Sampling the
// same ramp per fragment gets the look in one pass.
//
// The source is the baked ring texture, already tinted by loading_scene.frag,
// so this only blurs and gains -- tinting twice would shift the colour.

precision highp float;
precision highp sampler2D;

varying vec2 in_tex;

uniform sampler2D source;

// Point the streaks converge on, in texture space
uniform vec2 zoom_center;
// Horizontal position of the lit band, in texture space
uniform float sweep_x;
// How far the last tap is pulled toward the centre, e.g. 0.25
uniform float zoom_strength;
// Overall gain, since the accumulation is normalised
uniform float exposure;
// Darkening away from the lit band, as an ellipse in texture space:
// x is the narrow axis the sweep travels along, y is stretched so the band
// spans the screen instead of punching a circle through it. The decay itself
// was measured off the original as (1 - r)^0.6.
uniform vec2  falloff_extent;
uniform float falloff_power;

const int TAPS = 15;

// The band lights the frame before it is zoomed, so it is applied per tap and
// streaks outward from zoom_center along with the surface
float falloff(vec2 uv)
{
    vec2 band = vec2(sweep_x, zoom_center.y);
    // Distance measured in units of the ellipse's own axes, so 1.0 is its edge
    float radius = length((uv - band) / falloff_extent);
    return pow(max(0.0, 1.0 - radius), falloff_power);
}

void main()
{
    vec2  offset = in_tex - zoom_center;
    float total  = 0.0;
    vec3  accum  = vec3(0.0);

    for(int i = 0; i < TAPS; i++)
    {
        float t     = float(i) / float(TAPS - 1);
        float scale = 1.0 - zoom_strength * t;
        vec2  uv    = clamp(zoom_center + offset * scale, 0.0, 1.0);

        // Falling weight gives a comet trail rather than a flat smear
        float weight = 1.0 - t;

        accum += texture2D(source, uv).rgb * (weight * falloff(uv));
        total += weight;
    }

    gl_FragColor = vec4(accum / total * exposure, 1.0);
}
