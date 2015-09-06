#version 330

uniform mat4 camera;
uniform mat4 model;

// material settings
uniform struct Material {
	sampler2D diffuseSampler;

	vec3 transparencyValue;
	vec3 colorMultiplier;
} mtl;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 finalColor;

void main() {
    vec4 surfaceColor = texture(mtl.diffuseSampler, fragTexCoord);

    //diffuse
    vec3 diffuse = surfaceColor.rgb*mtl.colorMultiplier;

    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/1.3);
    finalColor = vec4(1.0,0.0,0.0,1.0);
}
