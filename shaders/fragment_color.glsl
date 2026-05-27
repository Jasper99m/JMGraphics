#version 330 core

out vec4 FragColor;
in vec4 fillColor;

uniform bool overrideColor;
uniform vec4 color;

void main() {
	if(overrideColor){
		FragColor = color;
	}
	else{
		FragColor = vec4(fillColor);
	}
}