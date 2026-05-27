#version 330 core

//the vertex array. (location = 0) sets the start index
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

//the transformation matrix
uniform mat4 transform;

//used to correct for window size
uniform mat4 windowTransform;

void main() {
	gl_Position = windowTransform * transform * vec4(aPos, 1.0f);

	texCoord = aTexCoord;
}