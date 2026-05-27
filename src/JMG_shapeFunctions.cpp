#include "../JMGraphics.h"


//just clears the screen and sets the backroung color
void JMGraphics::background(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}
void JMGraphics::background(float r, float g, float b) {
	background(r, g, b, 1.0f);
}
void JMGraphics::background(float v) {
	background(v, v, v, 1.0f);
}

void JMGraphics::arc(float x, float y, float sizeX, float sizeY, float startAngle, float endAngle, bool pie) {
	if (endAngle - startAngle > TWO_PI) {
		ellipse(x, y, sizeX, sizeY);
		return;
	}

	while (startAngle < 0.0f) {
		startAngle += TWO_PI;
		//std::cout << "Start angle too low." << std::endl;
	}
	while (endAngle < 0.0f) {
		endAngle += TWO_PI;
		//std::cout << "End angle too low." << std::endl;
	}



	if (endAngle < startAngle) {
		return;
	}
	else if (endAngle == startAngle) {
		return;
	}
	//the nunber of sides on a thoretical circle that the arc will be a portion of, plus the center vert.
	const int maxVerts = 64 + 1;

	//the actual number of verts to use in the arc
	int vertNum = (int)((endAngle - startAngle) * ((maxVerts - 1) / TWO_PI)) + 2;

	float vertices[maxVerts * 7];
	unsigned int indices[(maxVerts) * 3];
	unsigned int lineIndices[(maxVerts + 1) * 2];

	//create the first vertex at the center. needed to make triangles.
	vertices[0] = x;
	vertices[1] = y;
	vertices[2] = 0.0f;

	vertices[3] = fillColor.x;
	vertices[4] = fillColor.y;
	vertices[5] = fillColor.z;
	vertices[6] = fillColor.w;

	//create the second vertex at the start angle
	vertices[7] = x + std::cos(startAngle) * sizeX / 2;
	vertices[8] = y + std::sin(startAngle) * sizeY / 2;
	vertices[9] = 0.0f;

	vertices[10] = fillColor.x;
	vertices[11] = fillColor.y;
	vertices[12] = fillColor.z;
	vertices[13] = fillColor.w;


	//create the other vertices around the edge of the polygon.
	for (int i = 14; i < (vertNum * 7); i += 7) {
		float angle;
		if (i == (vertNum * 7) - 7) {
			angle = endAngle;
		}
		else {
			angle = startAngle + (((i - 7) / 7) * (TWO_PI / (maxVerts - 1)));
		}


		vertices[i] = x + std::cos(angle) * sizeX / 2;
		vertices[i + 1] = y + std::sin(angle) * sizeY / 2;
		vertices[i + 2] = 0.0f;

		vertices[i + 3] = fillColor.x;
		vertices[i + 4] = fillColor.y;
		vertices[i + 5] = fillColor.z;
		vertices[i + 6] = fillColor.w;
	}

	//create the array of indices.
	if (useFill) {
		for (int i = 0; i < vertNum - 1; i++) {
			indices[i * 3] = 0;

			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = i + 2;
		}

		//the last triangle uses the vert at index 0
		if (vertNum > 1) {
			indices[((vertNum - 1) * 3) - 1] = 0;
		}
	}

	//create a second array of indeces for drawing the outiline
	if (useStroke) {

		for (int i = 0; i < vertNum; i++) {
			lineIndices[i * 2] = i;
			lineIndices[i * 2 + 1] = i + 1;
		}

		if (vertNum > 1 && pie) {
			lineIndices[((vertNum) * 2) - 1] = 0;
		}
		else if (vertNum > 1) {
			lineIndices[0] = 1;
			lineIndices[((vertNum) * 2) - 1] = vertNum - 1;
		}
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	if (useFill) {
		glUniform1i(shader2DColorOverride, GL_FALSE);
		//set up EBO

		overwriteEBO(indices, sizeof(indices));
		//index to draw at, number of verts.
		glDrawElements(GL_TRIANGLES, vertNum * 3, GL_UNSIGNED_INT, 0);
	}
	if (useStroke) {
		if (roundedStroke) {
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}
		

		glUniform1i(shader2DColorOverride, GL_TRUE);
		overwriteEBO(lineIndices, sizeof(lineIndices));
		glDrawElements(GL_LINES, vertNum * 2, GL_UNSIGNED_INT, 0);
		glUniform1i(shader2DColorOverride, GL_FALSE);

		if (roundedStroke && strokeWeightValue / scaleFactor > 4.0f) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			//draw a point at each vertex
			for (int i = 1; i < vertNum; i++) {
				pushMatrix();
				translate(vertices[i * 7], vertices[i * 7 + 1]);
				shaderColor->setMat4("transform", tranformMat);
				point(0.0f, 0.0f);
				popMatrix();
			}

			if (pie) {
				point(x, y);
			}
			if (roundedStroke) {
				glDisable(GL_STENCIL_TEST);
			}
		}
	}

	
}

void JMGraphics::arc(float x, float y, float sizeX, float sizeY, float startAngle, float endAngle) {
	arc(x, y, sizeX, sizeY, startAngle, endAngle, false);
}

void JMGraphics::rect(float posX, float posY, float sizeX, float sizeY) {

	float vertices[]{
		//verts												//vert color
		posX - sizeX / 2, posY - sizeY / 2, 0.0f,			fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		posX + sizeX / 2, posY - sizeY / 2, 0.0f,			fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		posX + sizeX / 2, posY + sizeY / 2, 0.0f,			fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		posX - sizeX / 2, posY + sizeY / 2, 0.0f,			fillColor.x, fillColor.y, fillColor.z, fillColor.w
	};

	unsigned int indices[]{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int lineIndices[]{
		0, 1, 1, 2, 2, 3, 3, 0
	};
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));
	overwriteEBO(indices, sizeof(indices));
	
	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	if (useFill) {
		glUniform1i(shader2DColorOverride, GL_FALSE);
		
		//index to draw at, number of verts.
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
	}
	if (useStroke) {
		if (roundedStroke) {
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}

		glUniform1i(shader2DColorOverride, GL_TRUE);
		overwriteEBO(lineIndices, sizeof(lineIndices));
		glDrawElements(GL_LINES, sizeof(lineIndices) / sizeof(lineIndices[0]), GL_UNSIGNED_INT, 0);
		glUniform1i(shader2DColorOverride, GL_FALSE);

		if (roundedStroke && strokeWeightValue / scaleFactor > 3.0f) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			for (int i = 0; i < 4; i++) {
				point(vertices[i * 7], vertices[i * 7 + 1]);
			}
		}
		if (roundedStroke) {
			glDisable(GL_STENCIL_TEST);
		}
	}

}

void JMGraphics::rect(float x, float y, float sizeX, float sizeY, float radius) {
	//keep corners from overlapping
	if (radius > sizeX / 2) {
		radius = sizeX / 2;
	}
	else if (radius > sizeY / 2) {
		radius = sizeY / 2;
	}

	const int sides = 32;

	//the nunber of verticies to create. 1 is added so that the first vert can be the center.
	int vertNum = sides + 1;

	if (radius <= 0) {
		rect(x, y, sizeX, sizeY);
		return;
	}


	float vertices[(sides + 1) * 7];
	unsigned int indices[sides * 3];
	unsigned int lineIndices[sides * 2];

	//create the first vertex at the center. needed to make triangles.
	vertices[0] = x;
	vertices[1] = y;
	vertices[2] = 0.0f;

	vertices[3] = fillColor.x;
	vertices[4] = fillColor.y;
	vertices[5] = fillColor.z;
	vertices[6] = fillColor.w;

	//create the other vertices around the edge of the polygon.
	float cornerX = 0;
	float cornerY;
	for (int i = 7; i < (vertNum * 7); i += 7) {
		if ((i - 7) / 7 > sides * 0.5) {
			cornerY = y - (sizeY / 2 - radius);
		}
		else {
			cornerY = y + (sizeY / 2 - radius);
		}

		if ((i - 7) / 7 < sides * 0.25 || (i - 7) / 7 > sides * 0.75) {
			cornerX = x + (sizeX / 2 - radius);
		}
		else {
			cornerX = x - (sizeX / 2 - radius);
		}

		vertices[i] = cornerX + std::cos(((i - 7) / 7 * (TWO_PI / (vertNum - 1)))) * radius;
		vertices[i + 1] = cornerY + std::sin(((i - 7) / 7 * (TWO_PI / (vertNum - 1)))) * radius;
		vertices[i + 2] = 0.0f;

		vertices[i + 3] = fillColor.x;
		vertices[i + 4] = fillColor.y;
		vertices[i + 5] = fillColor.z;
		vertices[i + 6] = fillColor.w;
	}

	//create the array of indeces.
	if (useFill) {
		for (int i = 0; i < vertNum - 1; i++) {
			indices[i * 3] = 0;

			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = i + 2;
		}

		//the last triangle uses the vert at index 1
		indices[((vertNum - 1) * 3) - 1] = 1;
	}

	//create a second array of indeces for drawing the outiline
	if (useStroke) {
		for (int i = 0; i < vertNum - 1; i++) {
			lineIndices[i * 2] = i + 1;
			lineIndices[i * 2 + 1] = i + 2;
		}

		lineIndices[((vertNum - 1) * 2) - 1] = 1;
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	if (useFill) {
		glUniform1i(shader2DColorOverride, GL_FALSE);
		//set up EBO

		overwriteEBO(indices, sizeof(indices));
		//index to draw at, number of verts.
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
	}
	if (useStroke) {
		if (roundedStroke) {
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}

		glUniform1i(shader2DColorOverride, GL_TRUE);
		overwriteEBO(lineIndices, sizeof(lineIndices));
		glDrawElements(GL_LINES, sizeof(lineIndices) / sizeof(lineIndices[0]), GL_UNSIGNED_INT, 0);
		glUniform1i(shader2DColorOverride, GL_FALSE);

		if (roundedStroke && strokeWeightValue / scaleFactor > 4.0f) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			for (int i = 1; i < vertNum; i++) {
				pushMatrix();
				translate(vertices[i * 7], vertices[i * 7 + 1]);
				shaderColor->setMat4("transform", tranformMat);
				point(0.0f, 0.0f);
				popMatrix();
			}
		}
		if (roundedStroke) {
			glDisable(GL_STENCIL_TEST);
		}
	}
}

void JMGraphics::gradient(float posX, float posY, float sizeX, float sizeY, glm::vec3 color1, glm::vec3 color2) {
	gradient(posX, posY, sizeX, sizeY, glm::vec4(color1.x, color1.y, color1.z, 1.0f), glm::vec4(color2.x, color2.y, color2.z, 1.0f));
}

void JMGraphics::gradient(float posX, float posY, float sizeX, float sizeY, float color1, float color2) {
	gradient(posX, posY, sizeX, sizeY, glm::vec4(color1, color1, color1, 1.0f), glm::vec4(color2, color2, color2, 1.0f));
}

void JMGraphics::gradient(float posX, float posY, float sizeX, float sizeY, glm::vec4 color1, glm::vec4 color2) {
	float vertices[]{
		//verts												//vert color
		posX - sizeX / 2, posY - sizeY / 2, 0.0f,			color1.x, color1.y, color1.z, color1.w,
		posX + sizeX / 2, posY - sizeY / 2, 0.0f,			color2.x, color2.y, color2.z, color2.w,
		posX + sizeX / 2, posY + sizeY / 2, 0.0f,			color2.x, color2.y, color2.z, color2.w,
		posX - sizeX / 2, posY + sizeY / 2, 0.0f,			color1.x, color1.y, color1.z, color1.w
	};

	unsigned int indices[]{
		0, 1, 2,
		2, 3, 0
	};

	unsigned int lineIndices[]{
		0, 1, 1, 2, 2, 3, 3, 0
	};
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));
	overwriteEBO(indices, sizeof(indices));

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
}

void JMGraphics::line(float x1, float y1, float x2, float y2) {
	line(x1, y1, 0.0f, x2, y2, 0.0f);
}

void JMGraphics::line(float x1, float y1, float z1, float x2, float y2, float z2) {

	float vertices[]{
		x1, y1, z1,		0.0f, 0.0f, 0.0f, 0.0f,
		x2, y2, z2,		0.0f, 0.0f, 0.0f, 0.0f,
	};

	unsigned int indices[]{
		0, 1
	};

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);
	glUniform1i(shader2DColorOverride, GL_TRUE);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));
	overwriteEBO(indices, sizeof(indices));

	if (roundedStroke) {
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	glDrawElements(GL_LINES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

	if (roundedStroke && strokeWeightValue > 3.0f) {
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		point(x1, y1);
		point(x2, y2);
	}

	if (roundedStroke) {
		glDisable(GL_STENCIL_TEST);
	}

	glUniform1i(shader2DColorOverride, GL_FALSE);
}

void JMGraphics::dashedLine(float x1, float y1, float x2, float y2, float dashLength, float gapLength) {
	dashedLine(x1, y1, 0.0f, x2, y2, 0.0f, dashLength, gapLength);
}
void JMGraphics::dashedLine(float x1, float y1, float z1, float x2, float y2, float z2, float dashLength, float gapLength) {
	float lineLength = glm::distance(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z2));
	int vertNum = (int)(lineLength / (dashLength + gapLength)) * 2 + 2;
	const int maxVerts = 256;

	if (vertNum > maxVerts || vertNum < 2)
		return;

	float vertices[maxVerts * 3];
	unsigned int indices[maxVerts];

	glm::vec3 dir(glm::normalize(glm::vec3(x2, y2, z2) - glm::vec3(x1, y1, z1)));

	float pointX = x1;
	float pointY = y1;
	float pointZ = z1;
	for (int i = 0; i < vertNum; i++) {
		vertices[i * 3] = pointX;
		vertices[i * 3 + 1] = pointY;
		vertices[i * 3 + 2] = pointZ;

		if (i % 2 == 0) {
			pointX += dir.x * dashLength;
			pointY += dir.y * dashLength;
			pointZ += dir.z * dashLength;
		}
		else {
			pointX += dir.x * gapLength;
			pointY += dir.y * gapLength;
			pointZ += dir.z * gapLength;
		}

		indices[i] = i;
	}

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glUniform1i(shader2DColorOverride, GL_TRUE);

	overwriteVBO(vertices, sizeof(float) * vertNum * 3);
	overwriteEBO(indices, sizeof(unsigned int) * vertNum);

	glDrawElements(GL_LINES, vertNum, GL_UNSIGNED_INT, 0);

	glUniform1i(shader2DColorOverride, GL_FALSE);

}

void JMGraphics::point(float x, float y) {
	float vertices[17 * 7];
	unsigned int indices[16 * 3];

	vertices[0] = x;
	vertices[1] = y;
	vertices[2] = 0.0f;

	vertices[3] = 0.0f;
	vertices[4] = 0.0f;
	vertices[5] = 0.0f;
	vertices[6] = 0.0f;
	//vertex array
	for (int i = 7; i < 17 * 7; i += 7) {
		vertices[i] = x + cos((float)i / 7.0f * (TWO_PI / 16.0f)) * strokeWeightValue / 2.0f;
		vertices[i + 1] = y + sin((float)i / 7.0f * (TWO_PI / 16.0f)) * strokeWeightValue / 2.0f;
		vertices[i + 2] = 0.0f;

		vertices[i + 3] = 0.0f;
		vertices[i + 4] = 0.0f;
		vertices[i + 5] = 0.0f;
		vertices[i + 6] = 0.0f;
	}
	//index array
	for (int i = 0; i < 16; i++) {
		indices[i * 3] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;
	}
	indices[16 * 3 - 1] = 1;

	activateShader(shaderColor);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));
	overwriteEBO(indices, sizeof(indices));

	shaderColor->setMat4("transform", tranformMat);
	glUniform1i(shader2DColorOverride, GL_TRUE);
	glDrawElements(GL_TRIANGLES, 16 * 3, GL_UNSIGNED_INT, 0);
	glUniform1i(shader2DColorOverride, GL_FALSE);
}

void JMGraphics::box(float x, float y, float z) {
	float X = x / 2.0f;
	float Y = y / 2.0f;
	float Z = z / 2.0f;
	float vertices[]{
		X,  Y,  Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		X,  Y, -Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		X, -Y,  Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		X, -Y, -Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,

		-X,  Y,  Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		-X,  Y, -Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		-X, -Y,  Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w,
		-X, -Y, -Z,		fillColor.x, fillColor.y, fillColor.z, fillColor.w
	};

	/*
	In apposing face order on each axis.
	Clockwise verts.
	X:
	Face 1: 0,1,3,2
	Face 2: 5,4,6,7
	Y:
	Face 5: 1,0,4,5
	Face 6: 2,3,7,6
	Z:
	Face 3: 0,2,6,4
	Face 4: 1,5,7,3

	To make clockwise tris for face: relative 0,1,3 and 1,2,3
	*/

	unsigned int indices[]{
		0, 1, 2, 1, 3, 2,
		5, 4, 7, 4, 6, 7,
		1, 0, 5, 0, 4, 5,
		2, 3, 6, 3, 7, 6,
		0, 2, 4, 2, 6, 4,
		1, 5, 3, 5, 7, 3
	};

	//Lines for apposing faces on the X axis, then the four that connect them.
	unsigned int lineIndices[]{
		0, 1, 1, 3, 3, 2, 2, 0,
		5, 4, 4, 6, 6, 7, 7, 5,

		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	if (useFill) {
		glUniform1i(shader2DColorOverride, GL_FALSE);
		overwriteEBO(indices, sizeof(indices));
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
	}
	if (useStroke) {
		glUniform1i(shader2DColorOverride, GL_TRUE);
		overwriteEBO(lineIndices, sizeof(lineIndices));
		glDrawElements(GL_LINES, sizeof(lineIndices) / sizeof(lineIndices[0]), GL_UNSIGNED_INT, 0);
		glUniform1i(shader2DColorOverride, GL_FALSE);
	}
}

void JMGraphics::cube(float size) {
	box(size, size, size);
}

//draws a symetrical polygon at the given coordinates with the given number of sides.
void JMGraphics::polygon(float x, float y, float sizeX, float sizeY, int sides) {


	//the nunber of verticies to create. 1 is added so that the first vert can be the center.
	int vertNum = sides + 1;
	const int maxVerts = 256;

	//check if the number of sides can be created
	if (sides + 1 > maxVerts || sides < 3) {
		std::cerr << "JMGraphics::polygon() error: number of sides needs to be more than 2, and less than " << (maxVerts - 1) << std::endl;
		return;
	}

	float vertices[maxVerts * 7];
	unsigned int indices[(maxVerts - 1) * 3];
	unsigned int lineIndices[(maxVerts - 1) * 2];

	//create the first vertex at the center. needed to make triangles.
	vertices[0] = x;
	vertices[1] = y;
	vertices[2] = 0.0f;

	vertices[3] = fillColor.x;
	vertices[4] = fillColor.y;
	vertices[5] = fillColor.z;
	vertices[6] = fillColor.w;

	//create the other vertices around the edge of the polygon.
	for (int i = 7; i < (vertNum * 7); i += 7) {
		vertices[i] = x + std::cos((i / 7 * (TWO_PI / (vertNum - 1)))) * sizeX / 2;
		vertices[i + 1] = y + std::sin((i / 7 * (TWO_PI / (vertNum - 1)))) * sizeY / 2;
		vertices[i + 2] = 0.0f;

		vertices[i + 3] = fillColor.x;
		vertices[i + 4] = fillColor.y;
		vertices[i + 5] = fillColor.z;
		vertices[i + 6] = fillColor.w;
	}

	//create the array of indeces.
	if (useFill) {
		for (int i = 0; i < vertNum - 1; i++) {
			indices[i * 3] = 0;

			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = i + 2;
		}

		//the last triangle uses the vert at index 1
		indices[((vertNum - 1) * 3) - 1] = 1;
	}

	//create a second array of indeces for drawing the outiline
	if (useStroke) {
		for (int i = 0; i < vertNum - 1; i++) {
			lineIndices[i * 2] = i + 1;
			lineIndices[i * 2 + 1] = i + 2;
		}

		lineIndices[((vertNum - 1) * 2) - 1] = 1;
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	setAtribPoiner_default();

	overwriteVBO(vertices, sizeof(vertices));

	activateShader(shaderColor);
	shaderColor->setMat4("transform", tranformMat);

	if (useFill) {
		glUniform1i(shader2DColorOverride, GL_FALSE);
		//set up EBO

		overwriteEBO(indices, sizeof(indices));
		//index to draw at, number of verts.
		glDrawElements(GL_TRIANGLES, (vertNum - 1) * 3, GL_UNSIGNED_INT, 0);
	}
	if (useStroke) {
		if (roundedStroke) {
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}

		glUniform1i(shader2DColorOverride, GL_TRUE);
		overwriteEBO(lineIndices, sizeof(lineIndices));
		glDrawElements(GL_LINES, (vertNum - 1) * 2, GL_UNSIGNED_INT, 0);

		//draw circles on the ends of the lines
		if (roundedStroke && strokeWeightValue / scaleFactor > 3.0f) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			//draw a point at each vertex
			for (int i = 1; i < vertNum; i++) {
				pushMatrix();
				translate(vertices[i * 7], vertices[i * 7 + 1]);
				shaderColor->setMat4("transform", tranformMat);
				point(0.0f, 0.0f);
				popMatrix();
			}
		}
		if (roundedStroke) {
			glDisable(GL_STENCIL_TEST);
		}
	}
}

void JMGraphics::polygon(float x, float y, float radius, int sides) {
	polygon(x, y, radius, radius, sides);
}

void JMGraphics::ellipse(float x, float y, float sizeX, float sizeY) {
	polygon(x, y, sizeX, sizeY, 128);
}

void JMGraphics::ellipse(float x, float y, float diameter) {
	ellipse(x, y, diameter, diameter);
}