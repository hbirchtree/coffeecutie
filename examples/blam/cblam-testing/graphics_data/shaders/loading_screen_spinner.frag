#version 100

precision highp float;

varying vec2 in_tex;

uniform float range_start;

void main()
{
    float dist_from_center = length(in_tex * 1.3 - 0.65);
    if(dist_from_center > 0.6)
        discard;
    highp float alpha = sin(abs(dist_from_center) * 10.0 + 3.34) * 1.4 - 0.4;
    vec2 p1 = in_tex * 1.3 - 0.65;
    float p2_rad = range_start * 31.4;
    vec2 p2 = vec2(sin(p2_rad), cos(p2_rad));
    alpha = clamp(alpha, 0.0, 1.0) * (length(p2 - p1) / 4.0);
    alpha = pow(alpha * 2.5, 4.0);
    gl_FragColor = vec4(vec3(1.0), alpha);
}
