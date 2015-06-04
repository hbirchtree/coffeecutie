#version 330
in float inType;
in vec3 inPos;
in vec3 inVel;
in float inLife;

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
