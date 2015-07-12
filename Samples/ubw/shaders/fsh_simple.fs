#version 330

in vec2 uv;

out vec4 finalColor;

uniform sampler2D screen;
uniform float time;

void main(){
	finalColor = texture(screen,uv);
}
