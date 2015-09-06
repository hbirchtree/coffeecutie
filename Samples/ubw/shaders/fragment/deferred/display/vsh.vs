#version 330 core

layout(location = 0) in vec3 vert;
out vec2 uv;

void main(){
	gl_Position =  vec4(vert,1);
	uv = (vert.xy+vec2(1,1))/2.0;
}
