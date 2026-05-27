#include "../JMGraphics.h"

JMGraphics::JMGraphics(int windowWidth, int windowHeight, const char* title) {
	setupWindow(windowWidth, windowHeight, title, NULL, NULL);
}
JMGraphics::JMGraphics(int windowWidth, int windowHeight, const char* title, GLFWmonitor* monitor) {
	setupWindow(windowWidth, windowHeight, title, monitor, NULL);
}
JMGraphics::JMGraphics(int windowWidth, int windowHeight, const char* title, GLFWwindow* parentObject) {
	setupWindow(windowWidth, windowHeight, title, NULL, parentObject);
}
JMGraphics::JMGraphics(int windowWidth, int windowHeight, const char* title, GLFWmonitor* monitor, GLFWwindow* parentObject) {
	setupWindow(windowWidth, windowHeight, title, monitor, parentObject);
}

void JMGraphics::setupWindow(int windowWidth, int windowHeight, const char* title, GLFWmonitor* monitor, GLFWwindow* parentObject) {
	Width = windowWidth;
	Height = windowHeight;

	//OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwInit();

	//get monitor info
	monitors = glfwGetMonitors(&monitorCount);

	//required to work on apple systems
#	ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#	endif

	//create a new window
	window = glfwCreateWindow(Width, Height, title, NULL, parentObject);

	//check if the window was succesfully created
	if (window == NULL) {
		std::cerr << "JMGraphics Error: Could not create OpenGL window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwMakeContextCurrent(window);

	//init glad and check if it worked
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "JMGraphics Error: failed to initialize glad" << std::endl;
		glfwTerminate();
		return;
	}


	glViewport(0, 0, Width, Height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	for (int i = 0; i < 10; i++) { pressedKeys[i] = NULL; }

	//enables alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//set up the transformation matrices
	tranformMat = glm::mat4(1.0f);
	screenCoordMat = glm::mat4(1.0f);

	//-------------------------------------
	//set up the shaders
	shaderColor = new Shader("JMGraphics/shaders/vertex_color.glsl", "JMGraphics/shaders/fragment_color.glsl");
	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	shaderTex = new Shader("JMGraphics/shaders/vertex_texture.glsl", "JMGraphics/shaders/fragment_texture.glsl");
	activateShader(shaderTex);
	shaderTex->setMat4("transform", tranformMat);
	shaderTex->setBool("FXAA", true);

	shader2DFont = new Shader("JMGraphics/shaders/vertex_2d_font.glsl", "JMGraphics/shaders/fragment_2d_font.glsl");
	activateShader(shader2DFont);
	shader2DFont->setMat4("transform", tranformMat);

	shaderMesh = new Shader("JMGraphics/shaders/vertex_mesh.glsl", "JMGraphics/shaders/fragment_mesh.glsl");
	activateShader(shaderMesh);
	shaderMesh->setMat4("transfrom", tranformMat);
	shaderMesh->setInt("colorMode", 1);
	shaderMesh->set4Float("color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	shaderMesh->set4Float("wireframeColor", glm::vec4(0.0, 0.3, 1.0, 1.0));
	shaderMesh->setBool("wireframeMode", false);

	setScreenCoordMat();

	//set up the color vectors
	fillColor = glm::vec4(1.0, 0.0, 1.0, 1.0);
	strokeColor = glm::vec4(1.0, 0.0, 1.0, 1.0);

	useFill = true;
	useStroke = true;

	//used by the shape functions to override the color info imbeded in the vertex data.
	shader2DColorOverride = glGetUniformLocation(shaderColor->id, "overrideColor");
	shader2DColor = glGetUniformLocation(shaderColor->id, "color");

	//VAO = vertex array obj, VBO = vertex buffer obj.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//binds a vertex array (makes it the active one)
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//generate the buffers at a big enough size for any of the shape functions.
	for (int i = 0; i < initVertNum; i++) {
		initVertices[i] = 0.0f;
	}
	for (int i = 0; i < initIndexNum; i++) {
		initIndices[i] = 0;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * initVertNum, initVertices, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * initIndexNum, initIndices, GL_DYNAMIC_DRAW);

	VBOsize = sizeof(initVertices);
	EBOsize = sizeof(initIndices);

	//set to fullscreen if a monitor is specified
	if (monitor) {
		fullscreen(monitor);
	}

	beginDraw();
	endDraw();
}


JMGraphics:: ~JMGraphics() {
	glfwMakeContextCurrent(window);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glFinish();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	delete(shaderColor);
	delete(shaderTex);
	delete(shader2DFont);
	delete(shaderMesh);

	glfwMakeContextCurrent(NULL);
	glFinish();
	glfwDestroyWindow(window);
}

void JMGraphics::closeWindow() {
	glfwSetWindowShouldClose(window, true);
}

void JMGraphics::beginDraw() {
	IsRendering = true;
	glfwMakeContextCurrent(window);

	//bind the vao and buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glFlush();
}


void JMGraphics::endDraw() {

	while (PauseNum > 0) {
		glFlush();
		IsPaused = true;
		std::unique_lock<std::mutex> lock(privMtx);
		privPauseCondition.wait(lock);
	}
	IsPaused = false;

	IsRendering = false;

	processInput(window);

	if (Width != widthLast || Height != heightLast) {
		WindowResized = true;
		widthLast = Width;
		heightLast = Height;
	}
	else {
		WindowResized = false;
	}

	if (scaleFactorLast != scaleFactor) {
		ScaleFactorChanged = true;
		scaleFactorLast = scaleFactor;
	}
	else {
		ScaleFactorChanged = false;
	}

	//sends new frame from buffer to window
	glfwSwapBuffers(window);

	typedCharacters.clear();
	FrameTime = clock() - FrameStart;
	

	//limit the frame rate
	if (minFrameTime != 0.0f && FrameTime < minFrameTime) {
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(minFrameTime - FrameTime)));
		FrameTime = clock() - FrameStart;

		//adjust the minimum frame time to target the correct frame rate
		if (FrameTime > minFrameTimeTarget)
			minFrameTime *= 0.99f;
		else if (FrameTime < minFrameTimeTarget)
			minFrameTime *= 1.01f;
	}

	FrameStart = (float)clock();

	FrameRate = (1000.0f / FrameTime) * 0.5f + FrameRate * 0.5f;
	if (FrameRate >= 1000) {
		FrameRate = 1000;
	}
	
	glFlush();
	glfwPollEvents();
}

void JMGraphics::overwriteVBO(float vertices[], GLsizei arraySize) {

	if (arraySize > VBOsize) {
		glBufferData(GL_ARRAY_BUFFER, arraySize, vertices, GL_DYNAMIC_DRAW);
		VBOsize = arraySize;
		return;
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, arraySize, vertices);	
}

void JMGraphics::overwriteEBO(unsigned int indices[], GLsizei arraySize) {
	
	if (arraySize > EBOsize) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, arraySize, indices, GL_DYNAMIC_DRAW);
		EBOsize = arraySize;
		return;
	}
	
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, arraySize, indices);
}

void JMGraphics::setAtribPoiner_default() {
	//position pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color pointer
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void JMGraphics::activateShader(Shader* shader) {
	if (activeShader == shader)
		return;

	shader->activate();
	activeShader = shader;
}



float JMGraphics::smoothIncrease(float input, float scaler)  const {
	if (input >= 1.0f) {
		return 1.0f;
	}
	float output = input + (((1.0f - input) * (frameTime() / 1000.0f)) * scaler);
	if (output > 0.995f) {
		output = 1.0f;
	}
	return output;
}

float JMGraphics::smoothDecrease(float input, float scaler)  const {
	if (input <= 0.005f) {
		return 0.0f;
	}
	float output = input - ((input * (frameTime() / 1000.0f)) * scaler);
	if (output < 0.005f) {
		output = 0.0f;
	}
	return output;
}

float JMGraphics::linearIncrease(float input, float scaler)  const {
	if (input >= 1.0f) {
		return 1.0f;
	}
	float output = input + ((frameTime() / 1000.0f) * scaler);
	if (output > 1.0f) {
		output = 1.0f;
	}
	return output;
}
float JMGraphics::linearDecrease(float input, float scaler)  const {
	if (input <= 0.0f) {
		return 0.0f;
	}
	float output = input - ((frameTime() / 1000.0f) * scaler);
	if (output < 0.0f) {
		output = 0.0f;
	}
	return output;
}