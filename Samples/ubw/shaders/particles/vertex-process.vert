#version 330
layout(location = 0) in float inType;
layout(location = 1) in vec3 inPos;
layout(location = 2) in vec3 inVel;
layout(location = 3) in float inLife;

out float geoType;
out vec3 geoPos;
out vec3 geoVel;
out float geoLife;

void main() {
	geoType = inType;
    geoPos = inPos;
    geoVel = inVel;
    geoLife = inLife;
}
