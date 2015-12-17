#version 330




in v_block {
	vec3 vert;
	vec4 mVert;
	vec2 texCoord;
	vec3 normal;
	vec3 wNormal;
	vec3 wTangent;
	flat int iDrawID;
} In;


uniform sampler2D diffuseSampler;

layout(location = 0) out vec4 Out_color;

void main() {
	vec4 col = texture(diffuseSampler,In.texCoord);
//    Out_color = vec4(col.xyz*(1.0/1.3),col.a);
	Out_color = vec4(1.0);
}
