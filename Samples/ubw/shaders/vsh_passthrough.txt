#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vert;

// Output data ; will be interpolated for each fragment.
out vec2 uv;

void main(){
	gl_Position =  vec4(vert,1);
	uv = (vert.xy+vec2(1,1))/2.0;
}

