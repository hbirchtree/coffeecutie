#version 450 core

in FragData {
    vec2 tex;
} frag;

out vec4 out_color;

void main()
{
    out_color = vec4(frag.tex.xyy, 1);
}
