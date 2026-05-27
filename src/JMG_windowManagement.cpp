#include "../JMGraphics.h"

//------------------------------------------------------------------------------------------------------------------
//Gets window properties

bool JMGraphics::isPaused()  const {
	return IsPaused;
}
bool JMGraphics::isRendering()  const {
	return IsRendering;
}
float JMGraphics::width()  const {
	//glfwGetFramebufferSize(window, &Width, &Height);
	return (float)Width * scaleFactor;
}
float JMGraphics::height()  const {
	//glfwGetFramebufferSize(window, &Width, &Height);
	return (float)Height * scaleFactor;
}
int JMGraphics::windowHeight() {
	glfwGetWindowSize(window, &WindowWidth, &WindowHeight);
	return (int)WindowHeight;
}
int JMGraphics::windowWidth() {
	glfwGetWindowSize(window, &WindowWidth, &WindowHeight);
	return (int)WindowWidth;
}
float JMGraphics::frameRate()  const {
	return FrameRate;
}
float JMGraphics::frameTime()  const {
	return FrameTime;
}

int JMGraphics::displayWidth()  const {
	return displayWidth(glfwGetPrimaryMonitor());
}
int JMGraphics::displayHeight()  const {
	return displayHeight(glfwGetPrimaryMonitor());
}
int JMGraphics::displayWidth(int monitor)  const {
	if (monitor >= monitorCount) {
		std::cerr << "JMGraphics::displayWidth() Error: Monitor " << monitor << " does not exist. Can not get display width." << std::endl;
		return 0;
	}
	return displayWidth(monitors[monitor]);
}
int JMGraphics::displayHeight(int monitor)  const {
	if (monitor >= monitorCount) {
		std::cerr << "JMGraphics::displayHeight() Error: Monitor " << monitor << " does not exist. Can not get display height." << std::endl;
		return 0;
	}
	return displayHeight(monitors[monitor]);
}
int JMGraphics::displayWidth(GLFWmonitor* monitor)  const {
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	return mode->width;
}
int JMGraphics::displayHeight(GLFWmonitor* monitor)  const {
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	return mode->height;
}


int JMGraphics::workAreaWidth()  const {
	return workAreaWidth(glfwGetPrimaryMonitor());
}
int JMGraphics::workAreaHeight()  const {
	return workAreaHeight(glfwGetPrimaryMonitor());
}

int JMGraphics::workAreaWidth(int monitor)  const {
	if (monitor >= monitorCount) {
		std::cout << "Monitor " << monitor << " does not exist. Can not get work area width." << std::endl;
		return 0;
	}
	return workAreaWidth(monitors[monitor]);
}
int JMGraphics::workAreaHeight(int monitor)  const {
	if (monitor >= monitorCount) {
		std::cout << "Monitor " << monitor << " does not exist. Can not get work area width." << std::endl;
		return 0;
	}
	return workAreaHeight(monitors[monitor]);
}

int JMGraphics::workAreaWidth(GLFWmonitor* monitor)  const {
	int width;
	glfwGetMonitorWorkarea(monitor, NULL, NULL, &width, NULL);
	return width;
}
int JMGraphics::workAreaHeight(GLFWmonitor* monitor)  const {
	int height;
	glfwGetMonitorWorkarea(monitor, NULL, NULL, NULL, &height);
	return height;
}

bool JMGraphics::windowResized()  const {
	return WindowResized;
}

int JMGraphics::monitorNum()  const {
	return monitorCount;
}


bool JMGraphics::windowOpen()  const {
	return !glfwWindowShouldClose(window);
}

//------------------------------------------------------------------------------------------------------------------
//sets window properties

void JMGraphics::pauseInLoop() {
	PauseNum++;
}
void JMGraphics::resume() {
	if (PauseNum > 0)
		PauseNum--;
	privPauseCondition.notify_all();
}
void JMGraphics::forceResume() {
	PauseNum = 0;
	privPauseCondition.notify_all();
}
void JMGraphics::size(int x, int y) {
	glfwSetWindowSize(window, x, y);
	glFinish();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	//glfwGetFramebufferSize(window, &Width, &Height);
}
void JMGraphics::windowPosition(int x, int y) {
	glfwSetWindowPos(window, x, y);
}
void JMGraphics::maximize() {
	glfwMaximizeWindow(window);
	//glfwGetFramebufferSize(window, &Width, &Height);
}
void JMGraphics::minimize() {
	glfwIconifyWindow(window);
}
void JMGraphics::fullscreen() {
	fullscreen(glfwGetPrimaryMonitor());
}
void JMGraphics::fullscreen(int monitor) {
	if (monitor >= monitorCount) {
		std::cout << "Monitor " << monitor << "does not exist. Can't set fullscreen window." << std::endl;
		return;
	}
	fullscreen(monitors[monitor]);
	//glfwGetFramebufferSize(window, &Width, &Height);
}
void JMGraphics::fullscreen(GLFWmonitor* monitor) {
	fullscreen(monitor, displayWidth(monitor), displayHeight(monitor));
}
void JMGraphics::fullscreen(GLFWmonitor* monitor, int width, int height) {
	glfwMakeContextCurrent(window);
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (!mode) {
		std::cerr << "JMGraphics::fullscreen error: Could not get window mode." << std::endl;
		return;
	}
	glfwSetWindowMonitor(window, monitor, 0, 0, width, height, GLFW_DONT_CARE);
	//glfwGetFramebufferSize(window, &Width, &Height);
}

void JMGraphics::isResizeable(bool resizable) {
	if (resizable) {
		glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);
	}
	else {
		glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
	}
}

void JMGraphics::limitFrameRate(int rate) {
	minFrameTimeTarget = 1000.0f / (float)rate;
	minFrameTime = minFrameTimeTarget;
}

bool JMGraphics::scaleFactorChanged()  const {
	return ScaleFactorChanged;
}

//------------------------------------------------------------------------------------------------------------------
//
void JMGraphics::on_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	Width = width;
	Height = height;
	setScreenCoordMat();
	std::this_thread::sleep_for(std::chrono::milliseconds(3));
}