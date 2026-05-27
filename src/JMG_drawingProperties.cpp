#include "../JMGraphics.h"

void JMGraphics::stroke(float r, float g, float b, float a) {
	activateShader(shaderColor);
	useStroke = true;
	strokeColor = glm::vec4(r, g, b, a);
	glUniform4fv(shader2DColor, 1, &strokeColor[0]);
}
void JMGraphics::stroke(float r, float g, float b) {
	stroke(r, g, b, 1.0f);
}
void JMGraphics::stroke(float v) {
	stroke(v, v, v, 1.0f);
}
void JMGraphics::stroke(glm::vec4 color) {
	stroke(color.x, color.y, color.z, color.w);
}
void JMGraphics::stroke(glm::vec3 color) {
	stroke(color.x, color.y, color.z, 1.0f);
}

void JMGraphics::fill(float r, float g, float b, float a) {
	useFill = true;
	fillColor = glm::vec4(r, g, b, a);
}
void JMGraphics::fill(float r, float g, float b) {
	useFill = true;
	fillColor = glm::vec4(r, g, b, 1.0f);
}
void JMGraphics::fill(float v) {
	useFill = true;
	fillColor = glm::vec4(v, v, v, 1.0f);
}
void JMGraphics::fill(glm::vec4 color) {
	useFill = true;
	fillColor = color;
}
void JMGraphics::fill(glm::vec3 color) {
	useFill = true;
	fillColor = glm::vec4(color.x, color.y, color.z, 1.0f);
}


glm::vec4 JMGraphics::lerpColor(glm::vec4 colorA, glm::vec4 colorB, float factor) {
	if (factor > 1.0f) {
		factor = 1.0f;
	}
	else if (factor < 0.0f) {
		factor = 0.0f;
	}

	return glm::vec4(colorA.x * (1.0f - factor) + colorB.x * factor,
					 colorA.y * (1.0f - factor) + colorB.y * factor,
					 colorA.z * (1.0f - factor) + colorB.z * factor,
					 colorA.w * (1.0f - factor) + colorB.w * factor);
}

glm::vec4 JMGraphics::lerpColor(glm::vec4 colorA, glm::vec4 colorB, glm::vec4 colorC, float factor) {
	if (factor > 1.0f) {
		factor = 1.0f;
	}
	else if (factor < 0.0f) {
		factor = 0.0f;
	}

	glm::vec4 newC;
	if (factor < 0.5f) {
		newC = JMGraphics::lerpColor(colorA, colorB, factor * 2.0f);
	}
	else {
		newC = JMGraphics::lerpColor(colorB, colorC, factor * 2.0f - 1.0f);
	}

	return newC;
}


glm::vec4 JMGraphics::invertColor(glm::vec4 color) {
	return glm::vec4(1.0f - color.x, 1.0f - color.y, 1.0f - color.z, color.w);
}

std::string JMGraphics::cleanValString(float val) {
	std::ostringstream oss;
	oss << val;
	return oss.str();

}
std::string JMGraphics::cleanValString(double val) {
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

void JMGraphics::strokeWeight(float weight) {
	strokeWeightValue = weight;
	glLineWidth(weight / scaleFactor);
}

void JMGraphics::noFill() {
	useFill = false;
}

void JMGraphics::noStroke() {
	useStroke = false;
}