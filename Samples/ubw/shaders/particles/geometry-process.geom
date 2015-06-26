#version 330

layout(points) in;
layout(points, max_vertices=30) out;

uniform float mass;
uniform float spawncount;
uniform float timestep;
uniform float randRad;
uniform float randAmpDiff;

uniform float partSpread;

uniform vec3 gravity;

in float[] geoType;
in vec3[] geoPos;
in vec3[] geoVel;
in float[] geoLife;

out float outType;
out vec3 outPos;
out vec3 outVel;
out float outLife;

void main(){

	if(geoType[0] == 0.0){
		outType = geoType[0];
		outVel = geoVel[0];
		outPos = geoPos[0];
		outLife = geoLife[0];
		EmitVertex();
		
		for(int i=0;i<spawncount;i++){
			outType = 1.0;
			outVel = geoVel[0]+gravity*mass*timestep+vec3(vec3(1,0,0)*sin(randRad)+vec3(0,0,1)*cos(randRad))*randAmpDiff*partSpread;
			outPos = geoPos[0]+outVel*timestep;
			outLife = geoLife[0];
			EmitVertex();
		}
	}else if(geoType[0] == 1.0){
		outType = geoType[0];
		outVel = geoVel[0]+gravity*mass*timestep;
		outPos = geoPos[0]+outVel*timestep;
		outLife = geoLife[0]-timestep;
		
		if(outLife>0)
			EmitVertex();
	}

	EndPrimitive();
}
