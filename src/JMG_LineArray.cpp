#include "../JMGraphics.h"

JMGraphics::LineArray::LineArray(JMGraphics* Parent) {
	parent = Parent;
	glGenVertexArrays(1, &linesVAO);
	glBindVertexArray(linesVAO);

	glGenBuffers(1, &linesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

	//position pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color pointer
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	for (int i = 0; i < VBOsize; i++) {
		verts.push_back(0.0f);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VBOsize, verts.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

JMGraphics::LineArray::~LineArray() {
	glDeleteBuffers(1, &linesVBO);
	glDeleteVertexArrays(1, &linesVAO);
}

bool JMGraphics::LineArray::addLine(glm::vec3 start, glm::vec3 end) {
	return addLine(start, end, parent->strokeColor);
}
bool JMGraphics::LineArray::addLine(glm::vec3 start, glm::vec3 end, glm::vec4 color) {
	float newLine[14];

	newLine[0] = start.x;
	newLine[1] = start.y;
	newLine[2] = start.z;

	newLine[3] = color.x;
	newLine[4] = color.y;
	newLine[5] = color.z;
	newLine[6] = color.w;

	newLine[7] = end.x;
	newLine[8] = end.y;
	newLine[9] = end.z;

	newLine[10] = color.x;
	newLine[11] = color.y;
	newLine[12] = color.z;
	newLine[13] = color.w;

	int ind = lineNum * 14;
	for (int i = 0; i < 14; i++) {
		if (ind < verts.size()) {
			verts[ind] = newLine[i];
		}
		else {
			verts.push_back(newLine[i]);
		}
		ind++;
	}

	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

	if (verts.size() > VBOsize) {
		VBOsize += 1024;
		for (int i = (int)verts.size(); i < VBOsize; i++) {
			verts.push_back(0.0f);
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), verts.data(), GL_STATIC_DRAW);
	}

	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * lineNum * 14, sizeof(float) * 14, newLine);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	lineNum++;
	return true;
}

void JMGraphics::LineArray::clearLines() {
	lineNum = 0;
}

void JMGraphics::LineArray::display() {
	if (parent == nullptr || lineNum <= 1) {
		return;
	}

	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
	parent->activateShader(parent->shaderColor);
	parent->shaderColor->setMat4("transform", parent->tranformMat);
	glUniform1i(parent->shader2DColorOverride, GL_FALSE);
	glDrawArrays(GL_LINES, 0, lineNum * 2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int JMGraphics::LineArray::totalLines()  const {
	return lineNum;
}