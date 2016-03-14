#version 330 core
#extension ARB_cull_distance : enable

layout(location=0)in vec3 pos;
layout(location=1)in vec2 tex;

out gl_PerVertex{
    vec4 gl_Position;
    float gl_ClipDistance[1];
#ifdef ARB_cull_distance
    float gl_CullDistance[1];
#endif
};
out VS_OUT{
    vec2 tc;
    flat int instance;
} vs_out;

uniform mat4 transform[102];
uniform vec2 tex_mul[2];

void main(void)
{
    /* The current eye being rendered */
    int instance = gl_InstanceID;
    int eyeId = instance%2;

    /* Regular vertex properties */
    vs_out.instance = instance;
    vs_out.tc = tex*tex_mul[eyeId];
    gl_Position = transform[instance]*vec4(pos,1.0);

    const vec4 eyeClipEdge[2] = vec4[](vec4(-1.0,0.0,0.0,1.0),vec4(1.0,0.0,0.0,1.0));
    const float eyeOffsetScale[2] = float[](-0.5,0.5);

    vec4 clipPlane = vec4(0.0);
    clipPlane.x = eyeOffsetScale[eyeId];

    /* This lets us avoid having to fiddle with viewports or viewport arrays */
    gl_ClipDistance[0] = dot(gl_Position,clipPlane);

#ifdef ARB_cull_distance
    /* TODO: Fix culling, this could be important for perf */
    gl_CullDistance[0] = 1.0;
#endif
}
