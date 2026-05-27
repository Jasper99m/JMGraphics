#version 330 core

//the vertex array. (location = 0) sets the start index, 
//vec3 means 3 coords stored in aPos for each vert.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 fillColor;

uniform bool project;

//the transformation matrix
uniform mat4 transform;

//used for perspective projection if rendering in 3D
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
	

	fillColor = aColor;
}