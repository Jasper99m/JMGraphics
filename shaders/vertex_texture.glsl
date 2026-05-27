#version 330 core

//the vertex array. (location = 0) sets the start index
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform bool project;

out vec4 fillColor;
out vec2 texCoord;

//the transformation matrix
uniform mat4 transform;

uniform mat4 projection;

//used to correct for window size
uniform mat4 windowTransform;

void main() {
	if(project){
		gl_Position = projection * windowTransform * transform * vec4(aPos, 1.0f);
	}
	else{
		gl_Position = windowTransform * transform * vec4(aPos, 1.0f);
	}
	
	fillColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	texCoord = aTexCoord;
}