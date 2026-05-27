#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform vec4 color;
uniform sampler2D character;

void main() {
	vec4 sample = vec4(1.0, 1.0, 1.0, texture(character, texCoord).r);
	FragColor = vec4(color) * sample;
}