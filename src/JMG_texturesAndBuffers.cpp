#include "../JMGraphics.h"

//draws an image to the screen as a rectangle
void JMGraphics::displayTexture(GLuint texture, float x, float y, float sizeX, float sizeY) {


	float vertices[]{
		//verts										//texture coords
		std::roundf(x - sizeX / 2), std::roundf(y - sizeY / 2), 0.0f,			0.0f, 0.0f,
		std::roundf(x + sizeX / 2), std::roundf(y - sizeY / 2), 0.0f,			1.0f, 0.0f,
		std::roundf(x + sizeX / 2), std::roundf(y + sizeY / 2), 0.0f,			1.0f, 1.0f,
		std::roundf(x - sizeX / 2), std::roundf(y + sizeY / 2), 0.0f,			0.0f, 1.0f
	};

	unsigned int indices[]{
		0, 1, 2,
		2, 3, 0
	};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	overwriteVBO(vertices, sizeof(vertices));
	overwriteEBO(indices, sizeof(indices));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	activateShader(shaderTex);
	shaderTex->setMat4("transform", tranformMat);
	shaderTex->setInt("texture1", 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	int texWidth, texHeight;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

	//give the shader the texture size in pixels. used for FXAA.
	shaderTex->set2Float("textureSize", (float)texWidth, (float)texHeight);
	
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void JMGraphics::image(Buffer* buffer, float x, float y, float sizeX, float sizeY) {
	if (buffer->parent != this) {
		std::cerr << "JMGraphics::image Error: The Buffer object must be passed a reference to this JMGraphics object instance when initialized in order to draw it using image()." << std::endl;
		return;
	}
	displayTexture(buffer->texture, x, y, sizeX, sizeY);
}

void JMGraphics::image(Buffer* buffer, float x, float y) {
	image(buffer, x, y, buffer->width(), buffer->height());
}

void JMGraphics::image(Buffer3D* buffer, float x, float y, float sizeX, float sizeY) {
	if (buffer->parent != this) {
		std::cerr << "JMGraphics::image Error: The Buffer object must be passed a reference to this JMGraphics object instance when initialized in order to draw it using image()." << std::endl;
		return;
	}
	displayTexture(buffer->texture, x, y, sizeX, sizeY);
}

void JMGraphics::image(Buffer3D* buffer, float x, float y) {
	image(buffer, x, y, buffer->width(), buffer->height());
}

void JMGraphics::image(Texture* texture, float x, float y, float sizeX, float sizeY) {
	if (texture->texture == NULL) {
		std::cerr << "JMGraphics::image Error: No texture loaded. can't display image." << std::endl;
		return;
	}
	displayTexture(texture->texture, x, y, sizeX, sizeY);
}
void JMGraphics::image(Texture* texture, float x, float y) {
	image(texture, x, y, (float)texture->width(), (float)texture->height());
}



//------------------------------------------------------------------------------------------------------------------
//has an fbo and a linked texture. anything drawn between the beginDraw() and endDraw() functions is drawn to the buffer instead of the window
JMGraphics::Buffer::Buffer(float width, float height, JMGraphics* parentObject) {
	Width = width;
	Height = height;
	parent = parentObject;

	setup();
}

void JMGraphics::Buffer::setup() {
	if (parent == nullptr || parent->window == nullptr) {
		std::cerr << "JMGraphics::Buffer Error: Must initialize and setup the graphics object before initializing a buffer for it." << std::endl;
		return;
	}
	glfwMakeContextCurrent(parent->window);
	scaleFactorLast = parent->scaleFactor;

	// Create Framebuffer Object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLenum FBOerror = glGetError();

	//create the stencil buffer
	glGenRenderbuffers(1, &SB);
	glBindRenderbuffer(GL_RENDERBUFFER, SB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor));
	//attach stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, SB);

	GLenum SBerror = glGetError();

	// Create Texture
	createTexture();

	GLenum TextureError = glGetError();

	// Check for completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "-------------------------------------------------------------------------------------------" << std::endl;
		std::cerr << "JMGraphics::Buffer Error: Framebuffer is not complete! Status: " << status << std::endl;
		std::cerr << "FBO Error: " << FBOerror << std::endl;
		std::cerr << "Stencil Error: " << SBerror << std::endl;
		std::cerr << "Texture Error: " << TextureError << std::endl;
		std::cerr << "-------------------------------------------------------------------------------------------" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	createTexCoordMat();
}

void JMGraphics::Buffer::createTexCoordMat() {
	texCoordMat = glm::mat4(1.0f);
	texCoordMat = glm::scale(texCoordMat, glm::vec3((2.0f / Width), (2.0f / Height), 1.0f));
	texCoordMat = glm::translate(texCoordMat, glm::vec3((-Width / 2), (-Height / 2), 0.0f));
}

void JMGraphics::Buffer::setTexCoordMat() {
	parent->activateShader(parent->shaderColor);
	parent->shaderColor->setMat4("windowTransform", texCoordMat);
	parent->activateShader(parent->shaderTex);
	parent->shaderTex->setMat4("windowTransform", texCoordMat);
	parent->activateShader(parent->shader2DFont);
	parent->shader2DFont->setMat4("windowTransform", texCoordMat);
	parent->activateShader(parent->shaderMesh);
	parent->shaderMesh->setMat4("windowTransform", texCoordMat);
}

void JMGraphics::Buffer::createTexture() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	if (parent->bufferNearestFilter) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFinish();
}
JMGraphics::Buffer::~Buffer() {
	glDeleteRenderbuffers(1, &SB);
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &texture);
}

void JMGraphics::Buffer::setSize(float x, float y) {
	Width = x;
	Height = y;
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDeleteTextures(1, &texture);
	createTexture();
	createTexCoordMat();
	glFinish();
}

void JMGraphics::Buffer::beginDraw() {
	oldMatrix = parent->tranformMat;
	if (scaleFactorLast != parent->scaleFactor) {
		scaleFactorLast = parent->scaleFactor;
		setSize(Width, Height);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor));
	glBindRenderbuffer(GL_RENDERBUFFER, SB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor));
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	setTexCoordMat();
	isDrawing = true;
}
void JMGraphics::Buffer::endDraw() {
	parent->tranformMat = oldMatrix;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, parent->Width, parent->Height);
	parent->setScreenCoordMat();
	isDrawing = false;
}
float JMGraphics::Buffer::width()  const {
	return Width;
}
float JMGraphics::Buffer::height()  const {
	return Height;
}

bool JMGraphics::Buffer::isActive()  const {
	return isDrawing;
}


//------------------------------------------------------------------------------------------------------------------
JMGraphics::Buffer3D::Buffer3D(float width, float height, JMGraphics* parentObject) {
	Width = width;
	Height = height;
	parent = parentObject;

	if (parentObject == NULL || parentObject->window == NULL) {
		std::cerr << "JMGraphics::Buffer Error: Must initialize and setup the graphics object before initializing a buffer for it." << std::endl;
		return;
	}
	glfwMakeContextCurrent(parentObject->window);

	scaleFactorLast = parent->scaleFactor;

	// Create Framebuffer Object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLenum FBOerror = glGetError();

	//create the stencil buffer
	glGenRenderbuffers(1, &SB);
	glBindRenderbuffer(GL_RENDERBUFFER, SB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor));
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, SB);

	GLenum SBerror = glGetError();

	createTexture();
	GLenum TextureError = glGetError();

	// Check for completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "-------------------------------------------------------------------------------------------" << std::endl;
		std::cerr << "JMGraphics::Buffer3D Error: Framebuffer is not complete! Status: " << status << std::endl;
		std::cerr << "FBO Error: " << FBOerror << std::endl;
		std::cerr << "Stencil/Depth Error: " << SBerror << std::endl;
		std::cerr << "Texture Error: " << TextureError << std::endl;
		std::cerr << "-------------------------------------------------------------------------------------------" << std::endl;
	}

	createTexCoordMat();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 JMGraphics::Buffer3D::screenCoordMatrix() {
	return texCoordMat;
}

void JMGraphics::Buffer3D::createTexCoordMat() {
	texCoordMat = glm::mat4(1.0f);
	texCoordMat = glm::scale(texCoordMat, glm::vec3((2.0f / Width), (2.0f / Height), 1.0f));
	texCoordMat = glm::translate(texCoordMat, glm::vec3((-Width / 2), (-Height / 2), 0.0f));
}

void JMGraphics::Buffer3D::setTexCoordMat() {
	parent->activateShader(parent->shaderColor);
	parent->shaderColor->setMat4("windowTransform", texCoordMat);
	parent->activateShader(parent->shaderTex);
	parent->shaderTex->setMat4("windowTransform", texCoordMat);
	parent->activateShader(parent->shader2DFont);
	parent->shader2DFont->setMat4("windowTransform", texCoordMat);
	parent->activateShader(parent->shaderMesh);
	parent->shaderMesh->setMat4("windowTransform", texCoordMat);
}

void JMGraphics::Buffer3D::setProjectionMatrix() {
	projMat = glm::mat4(1.0f);

	if (perspectiveProjection) {
		//Aspect ratio needs to be 1.0f as it's already compensated for in the shader files using screenCoordMat.
		projMat = glm::perspective(fov, 1.0f, ClipStart, ClipEnd);
	}
	else {
		projMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -ClipEnd, ClipEnd);
	}

	parent->activateShader(parent->shaderColor);
	parent->shaderColor->setMat4("projection", projMat);
	parent->activateShader(parent->shaderTex);
	parent->shaderTex->setMat4("projection", projMat);
	parent->activateShader(parent->shaderMesh);
	parent->shaderMesh->setMat4("projection", projMat);
}

void JMGraphics::Buffer3D::clipStart(float value) {
	ClipStart = value;
}

void JMGraphics::Buffer3D::clipEnd(float value) {
	ClipEnd = value;
}

void JMGraphics::Buffer3D::setSize(float x, float y) {
	Width = x;
	Height = y;
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDeleteTextures(1, &texture);
	createTexture();
	createTexCoordMat();
	glFinish();
}

void JMGraphics::Buffer3D::createTexture() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	if (parent->bufferNearestFilter) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFinish();
}
JMGraphics::Buffer3D::~Buffer3D() {
	glDeleteRenderbuffers(1, &SB);
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &texture);
}

void JMGraphics::Buffer3D::beginDraw() {
	oldMatrix = parent->tranformMat;
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	if (scaleFactorLast != parent->scaleFactor) {
		setSize(Width, Height);
		scaleFactorLast = parent->scaleFactor;
	}
	

	glViewport(0, 0, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor));
	glBindRenderbuffer(GL_RENDERBUFFER, SB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)(Width / parent->scaleFactor), (GLsizei)(Height / parent->scaleFactor));

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	setTexCoordMat();
	parent->activateShader(parent->shaderColor);
	parent->shaderColor->setBool("project", true);
	parent->activateShader(parent->shaderTex);
	parent->shaderTex->setBool("project", true);
	setProjectionMatrix();
	isDrawing = true;
}
void JMGraphics::Buffer3D::endDraw() {
	parent->tranformMat = oldMatrix;
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, parent->Width, parent->Height);
	parent->setScreenCoordMat();
	parent->activateShader(parent->shaderColor);
	parent->shaderColor->setBool("project", false);
	parent->activateShader(parent->shaderTex);
	parent->shaderTex->setBool("project", false);
	isDrawing = false;
}
float JMGraphics::Buffer3D::width()  const {
	return Width;
}
float JMGraphics::Buffer3D::height()  const {
	return Height;
}

glm::mat4 JMGraphics::Buffer3D::projectionMatrix() {
	return projMat;
}

bool JMGraphics::Buffer3D::isActive()  const {
	return isDrawing;
}


//------------------------------------------------------------------------------------------------------------------
//loads and stores an image texture
JMGraphics::Texture::Texture() {

}

JMGraphics::Texture::Texture(std::string FilePath) {
	load(FilePath);
}

JMGraphics::Texture::~Texture() {
	glDeleteTextures(1, &texture);
}

void JMGraphics::Texture::load(std::string FilePath) {

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load(FilePath.c_str(), &Width, &Height, &nChanels, 0);

	//std::cout << "Image chanels at: " << filePath << " " << nChanels << std::endl;

	if (data) {
		if (nChanels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nChanels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "JMGraphics::Texture::load Error: Failed to load texture: " << filePath << std::endl;
	}

	stbi_image_free(data);
	filePath = filePath;
}

void JMGraphics::textureFxaa(bool FXAAon) {
	activateShader(shaderTex);
	shaderTex->setBool("FXAA", FXAAon);
}


float JMGraphics::Texture::width()  const {
	return (float)Width;
}

float JMGraphics::Texture::height()  const {
	return (float)Height;
}