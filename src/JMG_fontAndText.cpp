#include "../JMGraphics.h"


//------------------------------------------------------------------------------------------------------------------
//Text drawing stuff
void JMGraphics::setFont(Font* font) {
	currentFont = font;
}
void JMGraphics::textSize(float size) {
	TextSize = size;
}

//draws text to the screen
void JMGraphics::text(std::string text, float posX, float posY, bool leftJustify, glm::vec2 &textSize) {
	if (currentFont == NULL) {
		std::cout << "Must set a font before rendering text." << std::endl;
		return;
	}
	activateShader(shader2DFont);
	shader2DFont->setMat4("transform", tranformMat);
	shader2DFont->set4Float("color", fillColor);
	textSize = currentFont->render(shader2DFont, text, posX, posY, TextSize, leftJustify);
}
void JMGraphics::text(std::string text, float posX, float posY, glm::vec2& textSize) {
	this->text(text, posX, posY, false, textSize);
}

void JMGraphics::text(std::string text, float posX, float posY, bool leftJustify) {
	if (currentFont == NULL) {
		std::cout << "Must set a font before rendering text." << std::endl;
		return;
	}

	activateShader(shader2DFont);
	shader2DFont->setMat4("transform", tranformMat);
	shader2DFont->set4Float("color", fillColor);
	currentFont->render(shader2DFont, text, posX, posY, TextSize, leftJustify);
}
void JMGraphics::text(std::string text, float posX, float posY) {
	this->text(text, posX, posY, false);
}

//------------------------------------------------------------------------------------------------------------------
//loads and stores a font.
JMGraphics::Font::Font(JMGraphics* parentObject) {
	parent = parentObject;
}

bool JMGraphics::Font::load(std::string FilePath, int Size) {
	filePath = FilePath;
	size = Size;
	loadFromFile();


	glBindVertexArray(parent->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, parent->VBO);

	FT_Done_FreeType(parent->freeTypeLib);

	return true;
}

void JMGraphics::Font::loadFromFile() {
	//init freetype lib
	if (FT_Init_FreeType(&parent->freeTypeLib)) {
		std::cerr << "JMGraphics::Font Error: Failed to initialize freeType." << std::endl;
		return;
	}

	FT_Face fontFace;

	if (FT_New_Face(parent->freeTypeLib, filePath.c_str(), 0, &fontFace)) {
		std::cerr << "JMGraphics::Font Error: failed to load font: " << filePath << std::endl;
		return;
	}

	height = (int)((float)size / parent->scaleFactor);
	//set height and width
	FT_Set_Pixel_Sizes(fontFace, 0, height);

	//load texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //can use single byte per pixel

	glActiveTexture(GL_TEXTURE0);
	//load each glyph
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
			continue;
		}

		//gen texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			fontFace->glyph->bitmap.width,
			fontFace->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			fontFace->glyph->bitmap.buffer
		);

		//set tex params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//store character texture
		chars[c] = {
			texture,
			glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
			glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
			(unsigned int)fontFace->glyph->advance.x
		};
	}

	FT_Done_Face(fontFace);
}



glm::vec2 JMGraphics::Font::render(Shader* shader, std::string text, float x, float y, float scale, bool leftJust) {
	if (text.empty()) {
		return glm::vec2(0.0f, 0.0f);
	}

	glBindVertexArray(parent->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, parent->VBO);

	//re-load the font if the scale factor changes
	if (parent->scaleFactorChanged()) {
		loadFromFile();
	}
	glActiveTexture(GL_TEXTURE0);

	shader->setInt("character", 0);

	unsigned int indices[]{
		0, 1, 2,
		2, 3, 0
	};

	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
	parent->overwriteEBO(indices, sizeof(indices));


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//used to track the size of the text in pixels
	float xStart = 0.0f;
	float xEnd = 0.0f;
	float ySize = 0.0f;

	//go through the string
	for (int i = 0, len = (int)text.size(); i <= len; i++) {
		Character c;
		if(!leftJust){ c = chars[text[i]]; }
		else {
			c = chars[text[len - i]];
			if (text[len - i] != '\n') { x -= ((c.advance >> 6) * scale) * parent->scaleFactor; }
		}

		float width = std::roundf(c.size.x * scale);
		float height = std::roundf(c.size.y * scale);

		float xPos = std::roundf(x + (c.bearing.x * scale * parent->scaleFactor));
		float yPos = std::roundf(y - ((c.size.y - c.bearing.y) * scale * parent->scaleFactor));

		if (i == 0) {
			xStart = xPos;
			xEnd = xStart;
		}

		if (xPos + width > xEnd) {
			xEnd = xPos + width * parent->scaleFactor;
		}

		if (height * parent->scaleFactor > ySize) {
			ySize = height * parent->scaleFactor;
		}

		//setup quad to display texture
		float vertices[]{
			//verts		   																//texture coords
			xPos,												yPos + std::roundf(height * parent->scaleFactor),	0.0f, 0.0f, 0.0f,
			xPos + std::roundf(width * parent->scaleFactor),	yPos + std::roundf(height * parent->scaleFactor),	0.0f, 1.0f, 0.0f,
			xPos + std::roundf(width * parent->scaleFactor),	yPos,												0.0f, 1.0f, 1.0f,
			xPos,												yPos,												0.0f, 0.0f, 1.0f
		};

		//handle new line char or render the character.
		if ((text[i] == '\n' && !leftJust) || (text[len - i] == '\n' && leftJust)) {
			x = xStart;
			y -= c.size.y * 1.5f * parent->scaleFactor;

			ySize += c.size.y * 1.5f * parent->scaleFactor;
		}
		else {
			glBindTexture(GL_TEXTURE_2D, c.textureID);
			parent->overwriteVBO(vertices, sizeof(vertices));

			//draw the charecter
			glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

			//advance the cursor
			if (!leftJust) { x += ((c.advance >> 6) * scale) * parent->scaleFactor; }
		}
		
		

	}
	glBindTexture(GL_TEXTURE_2D, 0);

	return glm::vec2(abs(xEnd - xStart), ySize);
}

JMGraphics::Font::~Font() {
}