uniform sampler2D normal;
uniform samplerCube reflection;

in vec2 texc1;

in vec4 v_normal;
in vec4 v_binormal;
in vec4 v_tangent;

void main()
{
    vec4 mystery_src = vec4(v_normal.w, v_binormal.w, v_tangent.w, 0);
    vec4 base_color = texture(normal, texc1);
    normal = normal * 2 - 1;
    vec4 something;
    something.xyz = vec3(
        dot(v_normal.xyz, normal),
        dot(v_binormal.xyz, normal),
        dot(v_tangent.xyz, normal));
    something.w = dot(something.xyz, mystery_src.xyz);
//    mystery_src.w = dot();
    something.w = something.w / 1;
}
