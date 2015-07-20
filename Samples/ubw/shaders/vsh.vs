#version 330

//uniform ModelBlock {
	uniform mat4 model;
//};
//uniform CameraBlock {
	uniform mat4 camera,cameraVP;
//};

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 vertTexCoord;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBitangent;

out vec3 worldVert;
out vec2 localTexCoord;
out vec3 localNormal;
out vec3 worldNormal;
out vec3 worldTangent;
out vec4 surfacePos;

void main() {
    localTexCoord = vertTexCoord;
    vec4 mNormal = model * vec4(vertNormal,0.0);
    localNormal = normalize(mNormal).xyz;
    worldNormal = normalize((cameraVP * mNormal).xyz);
    vec4 worldVert_m = model * vec4(vert,1.0);
	vec4 worldVert_t = camera * worldVert_m;
    worldVert = worldVert_t.xyz;
    worldTangent = (model * vec4(vertTangent,0.0)).xyz;
    surfacePos = worldVert_t;

    gl_Position = worldVert_t;
}
