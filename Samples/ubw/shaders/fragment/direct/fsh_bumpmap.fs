#version 330

uniform vec3 cameraPosition;

// material settings
uniform struct Material {
	sampler2D diffuseSampler;
	sampler2D bumpSampler;
	sampler2D specularSampler;

	vec3 transparencyValue;
	float shininess;
	float shininessStrength;
	float opacity;
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	vec3 colorMultiplier;
} mtl;

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

vec3 bumpedNormal(){
    vec3 normal = 2.0 * texture(mtl.bumpSampler,localTexCoord).rgb - 1.0;
	vec3 tangent = worldTangent;
    vec3 bitang = cross(tangent,worldNormal);
    tangent = normalize(tangent - dot(tangent,localNormal)*localNormal);
    mat3 TBN = mat3(tangent,bitang,localNormal);
    normal = TBN * normal;
    normal = normalize(normal);

    return normal;
}

void main() {
    vec4 surfaceColor = texture(mtl.diffuseSampler, localTexCoord);
    vec3 surfaceToLight = normalize(light.position - surfacePos.xyz);
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePos.xyz);
    vec3 normal = bumpedNormal();

    vec4 specularValue = normalize(texture(mtl.specularSampler,localTexCoord)*2.0-1.0);
    vec3 pixelSpecularColor = specularValue.rgb;
    float pixelShininess = specularValue.a;

    //ambient
    vec3 ambient = light.ambientCoefficient * mtl.ambientColor * light.intensities;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;

    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), pixelShininess*mtl.shininess);
    vec3 specular = specularCoefficient * mtl.specularColor * pixelSpecularColor * light.intensities;

    //attenuation
    float distanceToLight = length(light.position - surfacePos.xyz);
    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation*(diffuse + specular);

    //gamme correction
    vec3 gamma = vec3(1.0/0.9);
    linearColor = pow(linearColor, gamma);

    //fog
    float fogCoord = abs(length(cameraPosition-surfacePos.xyz)/surfacePos.w);
    float fogVar = clamp(exp(-pow(fogParams.fDensity*fogCoord,7)),0.0,1.0);
    linearColor = mix(fogParams.fColor.xyz,linearColor,fogVar);

    //final color (after gamma correction)
    finalColor = vec4(linearColor, surfaceColor.a*mtl.opacity);
    finalNormal = vec4(normal,1.0);
    finalDiffuse = vec4(surfaceColor.rgb,surfaceColor.a*mtl.opacity);
    finalPosition = surfacePos;
    finalSpecular = vec4(specularCoefficient * mtl.specularColor,1.0);
}
