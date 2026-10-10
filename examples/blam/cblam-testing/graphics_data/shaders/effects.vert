#version 460 core

/* Sprites face the camera (or stretch along their axis); ribbons widen
 * across the view. Built here so one buffer serves every viewport. */

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 axis;
layout(location = 2) in vec4 corner; /* x, y in [-1, 1], z radius, w rotation */
layout(location = 3) in vec2 tex;
layout(location = 4) in vec4 color;
layout(location = 5) in vec4 atlas;  /* scale xy, offset zw */
layout(location = 6) in vec4 source; /* format, layer, kind (0 sprite, 1 ribbon, 2 flare), opaque */

layout(location = 0) uniform mat4 camera;
layout(location = 1) uniform vec3 eye;
layout(location = 2) uniform vec3 cam_right;
layout(location = 3) uniform vec3 cam_up;

layout(location = 0) out FragmentData
{
    vec2      tex;
    vec4      color;
    flat vec4 atlas;
    flat vec4 source;
} frag;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    vec3 world;
    if(source.z < 0.5)
    {
        float s = sin(corner.w);
        float c = cos(corner.w);
        vec2  at = vec2(c * corner.x - s * corner.y, s * corner.x + c * corner.y) *
                  corner.z;
        vec3 right = cam_right;
        vec3 up    = cam_up;
        float stretch = length(axis);
        if(stretch > 0.0001)
        {
            up    = axis / stretch;
            right = cross(up, eye - position);
            right = length(right) > 0.0001 ? normalize(right) : cam_right;
            /* Long side covers the distance moved, the short its radius */
            at = vec2(corner.x * corner.z, corner.y * max(corner.z, stretch * 0.5));
        }
        world = position + right * at.x + up * at.y;
    } else if(source.z < 1.5)
    {
        vec3 side = cross(axis, eye - position);
        side      = length(side) > 0.0001 ? normalize(side) : cam_right;
        world     = position + side * corner.x * corner.z;
    } else
    {
        /* Lens flares are already on screen; axis.x squares them up */
        vec2 at = position.xy + corner.xy * vec2(corner.z * axis.x, corner.z);
        gl_Position = vec4(at, 0.9999, 1.0);
        frag.tex    = tex;
        frag.color  = color;
        frag.atlas  = atlas;
        frag.source = source;
        return;
    }
    gl_Position  = camera * vec4(world, 1.0);
    frag.tex    = tex;
    frag.color  = color;
    frag.atlas  = atlas;
    frag.source = source;
}
