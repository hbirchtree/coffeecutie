#version 330

in vec2 uv;

out vec4 finalColor;

uniform float lightness;

uniform sampler2D diffuseSampler;
uniform sampler2D normalSampler;
uniform sampler2D specularSampler;
uniform sampler2D positionSampler;
uniform sampler2D depthSampler;
uniform sampler2D ambientSampler;
uniform sampler2D emissionSampler;

uniform struct Light {
   vec3 position;
   vec3 intensities; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;
} light;

uniform struct FogParams {
	float fDensity;
	vec4 fColor;
} fogParams;

uniform vec3 cameraPosition;

void main(){
	vec4 diffuse = texture(diffuseSampler,uv);
	vec3 normal = texture(normalSampler,uv).rgb;
	vec4 specular = texture(specularSampler,uv);
	vec4 position = texture(positionSampler,uv);

	float diffusion = max(0.0,dot(normalize(light.position-position.xyz),normal));

	vec3 linearColor = diffuse.xyz;

    finalColor = vec4(linearColor,1.0);
}
