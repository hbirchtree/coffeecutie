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

// material settings
layout(std430, binding=1) buffer sampler_data {
	vec3 diffuseSampler[];
};

layout(location = 0) out vec4 Out_color;

void main() {
//    vec4 surfaceColor = texture(mtl.diffuseSampler, fragTexCoord);

    //diffuse
//    vec3 diffuse = surfaceColor.rgb*mtl.colorMultiplier;

    //final color (after gamma correction)
//    vec3 gamma = vec3(1.0/1.3);
    Out_color = vec4(diffuseSampler[In.iDrawID],1.0);
}
