#version 330 core

out vec4 FragColor;

in vec4 fillColor;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D texture1;
uniform vec4 wireframeColor;
uniform bool wireframeMode;
uniform mat4 transform;

uniform int colorMode;
uniform vec4 color;

//Light directions
vec3 light1 = vec3(0.3, -0.8, 0.3);
vec3 light2 = vec3(-0.4, 0.4, -0.5);
vec3 light3 = vec3(0.1, 0.7, 0.3);

vec3 light1Color = vec3(1.0, 1.0, 1.0);
vec3 light2Color = vec3(1.0, 0.8, 0.75);
vec3 light3Color = vec3(0.65, 0.7, 1.0);

float light1Brightness = 0.8;
float light2Brightness = 0.5;
float light3Brightness = 0.3;

float ambientLight = 0.3;

void main() {

	if(wireframeMode){
		FragColor = wireframeColor;
		return;
	}
	if(colorMode == 0){
		FragColor = color;
	}
	else if(colorMode == 1){
		FragColor = vec4(fillColor);
	}
	else if(colorMode == 2){
		FragColor = texture(texture1, texCoord);
	}

	vec3 light1Dir = normalize(vec3(vec4(-light1, 0.0) * transform));
	vec3 light2Dir = normalize(vec3(vec4(-light2, 0.0) * transform));
	vec3 light3Dir = normalize(vec3(vec4(-light3, 0.0) * transform));

	vec3 norm = normalize(normal);
	float light1Scale = max(dot(norm, light1Dir), 0.0) * light1Brightness;
	float light2Scale = max(dot(norm, light2Dir), 0.0) * light2Brightness;
	float light3Scale = max(dot(norm, light3Dir), 0.0) * light3Brightness;

	vec3 lightTotal = vec3((light1Color * vec3(light1Scale)) + (light2Color * vec3(light2Scale)) + (light3Color * vec3(light3Scale)));
	FragColor *= vec4(lightTotal.rgb + vec3(ambientLight), 1.0);
}