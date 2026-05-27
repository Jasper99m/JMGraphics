#include "../JMG_shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	int success;
	char infoLog[512];

	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	//create a shader program, then link the vertex and fragment shaders
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragShader);
	glLinkProgram(id);

	//check if linking worked
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cerr << "JMGraphics Shader Error: Shader ID: " << id << " Linking error: " << std::endl << infoLog << std::endl;
	}

	//delete the shaders since they're now compiled and on the gpu
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

void Shader::activate() {
	glUseProgram(id);
}

//------------------------------------------------------------------------------------------------
//util functions

//loads the file path that contains the shaders
std::string Shader::loadShaderSrc(const char* filepath) {
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filepath);
	if (file.is_open()) {
		buf << file.rdbuf();
		ret = buf.str();
	}
	else {
		std::cerr << "JMGraphics Shader Error: Could not open shader source: " << filepath << std::endl;
	}
	
	file.close();

	return ret;

}

//compiles a shader .glsl file. the second argument should be the type of shader, ex: GL_VERTEX_SHADER
GLuint Shader::compileShader(const char* filepath, GLenum type) {
	int success;
	char infoLog[512];

	GLuint ret = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filepath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);

	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cerr << "JMGraphics Shader Error: Error with shader comp: " << std::endl << infoLog << std::endl;
	}
	
	return ret;
}


//------------------------------------------------------------------------------------------------
//uniform functions

//sets the shader transform matrix
//args: name (the name of the matrix in the vertex shader), matrix.

void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set2Float(const std::string& name, float v1, float v2) {
	glUniform2f(glGetUniformLocation(id, name.c_str()), v1, v2);
}

void Shader::set2Float(const std::string& name, glm::vec2 v) {
	glUniform2f(glGetUniformLocation(id, name.c_str()), v.x, v.y);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::set4Float(const std::string& name, glm::vec4 v) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setMat4(const std::string& name, glm::mat4 val) {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}