#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

	//Makes this the active shader to render with.
	void activate();

	//util functions

	//loads a .glsl file
	std::string loadShaderSrc(const char* filepath);

	//compiles the loaded .glsl file.
	GLuint compileShader(const char* filepath, GLenum type);

	//uniform functions
	void setBool(const std::string& name, bool value);

	void setInt(const std::string& name, int value);

	void setFloat(const std::string& name, float value);

	void set3Float(const std::string& name, float v1, float v2, float v3);

	void set2Float(const std::string& name, float v1, float v2);

	void set3Float(const std::string& name, glm::vec3 v);

	void set2Float(const std::string& name, glm::vec2 v);

	void set4Float(const std::string& name, float v1, float v2, float v3, float v4);

	void set4Float(const std::string& name, glm::vec4 v);

	void setMat3(const std::string& name, glm::mat3 val);

	void setMat4(const std::string& name, glm::mat4 val);

};