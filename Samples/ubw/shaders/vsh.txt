#version 330

uniform mat4 camera,model,cameraVP;

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
    localNormal = normalize(model * vec4(vertNormal,0.0)).xyz;
    worldNormal = normalize((cameraVP * model * vec4(vertNormal,0.0)).xyz);
	vec4 worldVert_t = camera * model * vec4(vert,1);
    worldVert = worldVert_t.xyz;
    worldTangent = (model * vec4(vertTangent,0.0)).xyz;
    surfacePos = model * vec4(vert,1.0);
    
    gl_Position = worldVert_t;
}
