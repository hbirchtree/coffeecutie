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

//	linearColor.z = 1.0-linearColor.z;

//	vec3 blurCol = vec3(0.0);

//	for(float i=0.0;i<10.0;i++){
//		vec2 target = vec2(i-5.0);
//		vec2 _uv = uv+target;
//		float dist = abs(dot(target,vec2(0.0)));
//		blurCol += texture(diffuseSampler,_uv).rgb/lightness;
//	}

//	linearColor = blurCol;


//    float fogCoord = abs(length(cameraPosition-position.xyz)/position.w);
//    float fogVar = clamp(exp(-pow(fogParams.fDensity*fogCoord,7)),0.0,1.0);
//    linearColor = mix(fogParams.fColor.rgb,linearColor,fogVar);


    finalColor = vec4(linearColor,1.0);
}
