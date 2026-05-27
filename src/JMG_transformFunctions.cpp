#include "../JMGraphics.h"

//------------------------------------------------------------------------------------------------------------------
//matrix transformation stuff
void JMGraphics::pushMatrix() {
	transMat2DStack.push(tranformMat);
}
void JMGraphics::popMatrix() {
	if (transMat2DStack.empty()) {
		std::cerr << "popMatrix() Error: Can't pop tranform matrix. Stack is empty." << std::endl;
		return;
	}
	tranformMat = transMat2DStack.top();
	transMat2DStack.pop();
}
void JMGraphics::resetMatrix() {
	tranformMat = glm::mat4(1.0f);
}
void JMGraphics::multiplyMatrix(glm::mat4 matrix) {
	tranformMat = tranformMat * matrix;
}
void JMGraphics::rotate(float angle) {
	tranformMat = glm::rotate(tranformMat, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}
void JMGraphics::rotate(float angle, glm::vec3 axis) {
	tranformMat = glm::rotate(tranformMat, angle, axis);
}
void JMGraphics::rotate(float X, float Y, float Z) {
	rotateX(X);
	rotateY(Y);
	rotateZ(Z);
}
void JMGraphics::rotateX(float angle) {
	tranformMat = glm::rotate(tranformMat, angle, glm::vec3(1.0f, 0.0f, 0.0f));
}
void JMGraphics::rotateY(float angle) {
	tranformMat = glm::rotate(tranformMat, angle, glm::vec3(0.0f, 1.0f, 0.0f));
}
void JMGraphics::rotateZ(float angle) {
	tranformMat = glm::rotate(tranformMat, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}
void JMGraphics::translate(float x, float y, float z) {
	tranformMat = glm::translate(tranformMat, glm::vec3(x, y, z));
}
void JMGraphics::translate(float x, float y) {
	JMGraphics::translate(x, y, 0.0f);
}
void JMGraphics::scale(float x, float y, float z) {
	tranformMat = glm::scale(tranformMat, glm::vec3(x, y, z));
}
void JMGraphics::scale(float x, float y) {
	tranformMat = glm::scale(tranformMat, glm::vec3(x, y, 0.0f));
}

glm::mat4 JMGraphics::transformationMatrix()  const {
	return tranformMat;
}
glm::mat4 JMGraphics::screenCoordMatrix()  const {
	return screenCoordMat;
}


//------------------------------------------------------------------------------------------------------------------
//re-creates and sets the windowTransform matrix in a given shader (shader must have this matrix). used to scale to window or buffer dimensions in pixels.
void JMGraphics::setScreenCoordMat() {
	screenCoordMat = glm::mat4(1.0f);
	screenCoordMat = glm::scale(screenCoordMat, glm::vec3((2.0f / width()),( 2.0f / height()), 1.0f));
	screenCoordMat = glm::translate(screenCoordMat, glm::vec3((-width() / 2.0f), ( - height() / 2.0f), 0.0f));
	activateShader(shaderColor);
	shaderColor->setMat4("windowTransform", screenCoordMat);

	activateShader(shaderTex);
	shaderTex->setMat4("windowTransform", screenCoordMat);

	activateShader(shader2DFont);
	shader2DFont->setMat4("windowTransform", screenCoordMat);

	activateShader(shaderMesh);
	shaderMesh->setMat4("windowTransfrom", screenCoordMat);
}