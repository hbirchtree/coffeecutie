#version 410
#extension GL_ARB_shader_storage_buffer_object : require
#extension GL_ARB_shader_draw_parameters : require

layout(std140) uniform MatrixBlock {
	mat4 camera,cameraVP;
};

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 vertTexCoord;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBitangent;
layout(location = 5) in mat4 model;

out v_block {
	vec3 vert;
	vec4 mVert;
	vec2 texCoord;
	vec3 normal;
	vec3 wNormal;
	vec3 wTangent;
	flat int iDrawID;
} Out;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() {
    Out.texCoord = vertTexCoord;
    Out.normal = normalize(model * vec4(vertNormal,0.0)).xyz;
    Out.wNormal = normalize((cameraVP * model * vec4(vertNormal,0.0)).xyz);
	vec4 worldVert_t = camera * model * vec4(vert,1);
    Out.vert = worldVert_t.xyz;
    Out.wTangent = (camera * model * vec4(vertTangent,0.0)).xyz;
    Out.mVert = model * vec4(vert,1.0);

    gl_Position = worldVert_t;
    Out.iDrawID = gl_DrawIDARB;
}
