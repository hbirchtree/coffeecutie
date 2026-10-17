#version 460 core
layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 color;

layout(location=0) out vec2 f_texcoord;
layout(location=1) out vec4 f_color;

/* Where the light is on screen, 0..1 */
layout(location=0) uniform vec2 light_uv;
layout(location=1, binding=1) uniform sampler2D scene_depth;

void main()
{
    /* The light shows where nothing wrote depth (reversed z clears to 0) */
    float seen = 0.0;
    for(int x = -1; x <= 1; x++)
        for(int y = -1; y <= 1; y++)
            seen += textureLod(scene_depth, light_uv + vec2(x, y) * 0.004, 0.0).r > 0.0
                ? 0.0 : 1.0 / 9.0;
    f_texcoord  = texcoord;
    f_color     = vec4(color.rgb * seen, color.a);
    gl_Position = vec4(position, 0.0, 1.0);
}
