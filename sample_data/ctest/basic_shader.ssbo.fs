#version 410
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_storage_buffer_object : require
#extension GL_ARB_shader_image_load_store : require

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
