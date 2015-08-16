#version 330

//uniform vec3 cameraPosition;

uniform struct MaterialStruct {
	sampler2D diffuseSampler;
} mtl;

uniform MaterialBlock {
	vec3 transparencyValue;
	float shininess;
	float shininessStrength;
	float opacity;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	vec3 colorMultiplier;
};

//uniform LightBlock {
//   vec3 position;
//   vec3 intensities; //a.k.a the color of the light
//   float attenuation;
//   float ambientCoefficient;
//};

//uniform FogParams {
//	float fDensity;
//	vec4 fColor;
//} fogParams;

in vec2 localTexCoord;
in vec3 worldNormal;
in vec3 localNormal;
in vec3 worldVert;
in vec3 worldTangent;
in vec4 surfacePos;

layout(location = 0) out vec4 finalColor;
layout(location = 1) out vec4 finalNormal;
layout(location = 2) out vec4 finalDiffuse;
layout(location = 3) out vec4 finalPosition;
layout(location = 4) out vec4 finalSpecular;
layout(location = 5) out vec4 finalAmbient;

void main() {
    vec4 surfaceColor = texture(mtl.diffuseSampler, localTexCoord);

    finalNormal = vec4(worldNormal,1.0);
    finalDiffuse = vec4((surfaceColor.rgb*diffuseColor)*colorMultiplier,1.0);
    finalPosition = surfacePos;
    finalSpecular = vec4(specularColor,shininess);
    finalAmbient = vec4(ambientColor,0.0);
}
    /*
    vec3 surfaceToLight = normalize(light.position - surfacePos.xyz);
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePos.xyz);

    //ambient
    vec3 ambient = light.ambientCoefficient * mtl.ambientColor * light.intensities;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(localNormal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;

    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, localNormal))), mtl.shininess);
    vec3 specular = specularCoefficient * mtl.specularColor * light.intensities;

    //attenuation
    float distanceToLight = length(light.position - surfacePos.xyz);
    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation*(diffuse + specular);

    //gamme correction
    vec3 gamma = vec3(1.0/1.3);
    linearColor = pow(linearColor, gamma);

    //fog
    float fogCoord = abs(length(cameraPosition-surfacePos.xyz)/surfacePos.w);
    float fogVar = clamp(exp(-pow(fogParams.fDensity*fogCoord,7)),0.0,1.0);
    linearColor = mix(fogParams.fColor.xyz,linearColor,fogVar);

    //final color (after gamma correction)
    finalColor = vec4(linearColor, surfaceColor.a*mtl.opacity);
    */
