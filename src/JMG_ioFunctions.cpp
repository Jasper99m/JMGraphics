#include "../JMGraphics.h"

//------------------------------------------------------------------------------------------------------------------
//processes user inputs
void JMGraphics::processInput(GLFWwindow* window) {

	MouseXLast = MouseX;
	MouseYLast = MouseY;

	glfwGetCursorPos(window, &MouseX, &MouseY);
	MouseY *= -1;
	MouseY += Height;

	MouseLeftDoubleCLicked = false;
	MouseRightDoubleClicked = false;

	if (mousePressed()) {
		MouseLeftDoubleCLicked = mouseLeftTimer < doubleClickTime && !mousePressedLeftLast;
		mousePressedLeftLast = true;
		mouseLeftTimer = 0.0f;
	}
	else {
		mousePressedLeftLast = false;
	}
	if (mousePressedRight()) {
		MouseRightDoubleClicked = mouseRightTimer < doubleClickTime && !mousePressedRightLast;
		mousePressedRightLast = true;
		mouseRightTimer = 0.0f;
	}
	else {
		mousePressedRightLast = false;
	}

	//increment the double click timer
	mouseLeftTimer += FrameTime;
	mouseRightTimer += FrameTime;


	if (!scrolledLast) {
		ScrollX = 0;
		ScrollY = 0;
	}
	else {
		scrolledLast = false;
	}

	pressedKey = 0;
	releasedKey = 0;
}

bool JMGraphics::mouseMoved()  const {
	if (MouseX == MouseXLast && MouseY == MouseYLast) {
		return false;
	}
	return true;
}
float JMGraphics::mouseX()  const {
	return (float)MouseX * scaleFactor;
}
float JMGraphics::mouseY()  const {
	return (float)MouseY * scaleFactor;
}

float JMGraphics::realMouseX()  const {
	return (float)MouseX;
}
float JMGraphics::realMouseY()  const {
	return (float)MouseY;
}

bool JMGraphics::mousePressed()  const {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		return true;
	}
	return false;
}
bool JMGraphics::mousePressedRight()  const {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
		return true;
	}
	return false;
}
bool JMGraphics::mouseClicked()  const {
	if (!mousePressed() && mousePressedLeftLast) {
		return true;
	}
	return false;
}

bool JMGraphics::mouseDoubleClicked()  const {
	return MouseLeftDoubleCLicked;
}
bool JMGraphics::mouseRightDoubleClicked()  const {
	return MouseRightDoubleClicked;
}
bool JMGraphics::mouseClickedRight()  const {
	if (!mousePressedRight() && mousePressedRightLast) {
		return true;
	}
	return false;
}
bool JMGraphics::mousePressedLast()  const {
	return mousePressedLeftLast;
}
bool JMGraphics::mouseRightPressedLast()  const {
	return mousePressedRightLast;
}

int JMGraphics::globalMouseX()  const {
	int windowPosX, windowPosY;
	glfwGetWindowPos(window, &windowPosX, &windowPosY);
	return (int)mouseX() + windowPosX;
}
int JMGraphics::globalMouseY()  const {
	int windowPosX, windowPosY;
	glfwGetWindowPos(window, &windowPosX, &windowPosY);
	return (int)(height() - mouseY()) + windowPosY;
}

float JMGraphics::scrollX()  const {
	return ScrollX;
}
float JMGraphics::scrollY()  const {
	return ScrollY;
}

bool JMGraphics::ScrolledLast()  const {
	return scrolledLast;
}

bool JMGraphics::keyDown(int key)  const {
	for (int i = 0; i < 10; i++) {
		if (key == pressedKeys[i]) {
			return true;
		}
	}
	return false;
}

bool JMGraphics::keyPressed(int key)  const {
	return key == pressedKey;
}

bool JMGraphics::keyReleased(int key)  const {
	return key == releasedKey;
}

bool JMGraphics::controlKeyDown()  const {
	return (keyDown(GLFW_KEY_LEFT_CONTROL) || keyDown(GLFW_KEY_RIGHT_CONTROL));
}
bool JMGraphics::altKeyDown()  const {
	return (keyDown(GLFW_KEY_LEFT_ALT) || keyDown(GLFW_KEY_RIGHT_ALT));
}
bool JMGraphics::shiftKeyDown()  const {
	return (keyDown(GLFW_KEY_LEFT_SHIFT) || keyDown(GLFW_KEY_RIGHT_SHIFT));
}

void JMGraphics::on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//add or remove the key from the array of currently pressed keys
	if (action == GLFW_PRESS) {
		pressedKey = key;
		for (int i = 0; i < 10; i++) {
			if (pressedKeys[i] == NULL) {
				pressedKeys[i] = key;
				break;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		releasedKey = key;
		for (int i = 0; i < 10; i++) {
			if (pressedKeys[i] == key) {
				pressedKeys[i] = NULL;
				break;
			}
		}
	}
}

void JMGraphics::on_character_callback(GLFWwindow* window, unsigned int codepoint) {
	typedCharacters.push_back((char)codepoint);
}

void JMGraphics::on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	ScrollX = (float)xoffset;
	ScrollY = (float)yoffset;
	scrolledLast = true;
}