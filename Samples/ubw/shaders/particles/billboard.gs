#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 modelview;
uniform vec3 cameraPos;
uniform float particleSize;

out vec2 TexCoord;

void main()
{
    vec3 Pos = gl_in[0].gl_Position.xyz;
    vec3 toCamera = normalize(cameraPos - Pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up) * particleSize;

    Pos -= right;
    gl_Position = modelview * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    Pos.y += particleSize;
    gl_Position = modelview * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    Pos.y -= particleSize;
    Pos += right;
    gl_Position = modelview * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    Pos.y += particleSize;
    gl_Position = modelview * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
