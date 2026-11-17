#version 100

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec2 in_tex;
varying vec3 in_world;

uniform float time;     /* s since the ring spawned */
uniform float strength; /* Fades it out */
uniform vec3  eye;
/* The ring's world axes: across, up and out of its face */
uniform vec3 axis_t;
uniform vec3 axis_b;
uniform vec3 axis_n;

const vec3 gold = vec3(1.0, 0.76, 0.33);

/* Stands in for a reflection map: warm sky over dark ground, a sun glint */
vec3 environment(vec3 dir)
{
    vec3  sky = mix(vec3(0.55, 0.42, 0.3), vec3(1.0, 0.95, 0.85),
                    smoothstep(0.0, 0.8, dir.z));
    vec3  col = mix(vec3(0.35, 0.26, 0.16), sky, smoothstep(-0.25, 0.15, dir.z));
    float sun = pow(max(dot(dir, normalize(vec3(0.4, 0.3, 0.85))), 0.0), 120.0);
    return col + vec3(4.0) * sun;
}

void main()
{
    vec2  d = in_tex * 2.0 - 1.0;
    float r = length(d);
    if(r > 1.0)
        discard;
    vec2 radial = r > 0.0001 ? d / r : vec2(0.0);

    /* Ripples running out from the centre since spawn, calming to the edge */
    float phase = r * 24.0 - time * 8.0;
    float front = step(r, time * 8.0 / 24.0);
    float calm  = 1.0 - r;
    float slope = cos(phase) * 0.6 * calm * front;

    vec3 v = normalize(eye - in_world);
    vec3 n = normalize(axis_n - (axis_t * radial.x + axis_b * radial.y) * slope);
    /* Seen from behind: the ripples mirror through the face */
    if(dot(axis_n, v) < 0.0)
        n -= 2.0 * dot(n, axis_n) * axis_n;

    float facing  = abs(dot(n, v));
    vec3  fresnel = gold + (1.0 - gold) * pow(1.0 - facing, 5.0);
    vec3  col     = environment(reflect(-v, n)) * fresnel;

    /* Densest in the middle, gone at the edge; crests stand out */
    float crest = 0.5 + 0.5 * sin(phase);
    float body  = 1.0 - smoothstep(0.0, 1.0, r);
    float alpha = body * (0.55 + 0.45 * crest * front) * strength;
    gl_FragColor = vec4(col * alpha, alpha);
}
