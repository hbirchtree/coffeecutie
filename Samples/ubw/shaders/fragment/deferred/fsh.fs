#version 330

uniform struct MaterialStruct {
	sampler2D diffuseSampler;
} mtl;


uniform vec3 transparencyValue;
uniform float shininess;
uniform float shininessStrength;
uniform float opacity;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 ambientColor;
uniform vec3 colorMultiplier;


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
