#pragma once


#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>

#define GLFW_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include <stack>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "JMG_shader.h"

#define PI 3.14159265f
#define TWO_PI 6.28318531f
#define HALF_PI 1.57079633f
#define QUARTER_PI 0.78539816f

/**
* This class is the core of the graphics engine. One instance of this class will open one window,
* and calling this instances functions will render to that window.\n
* Example of a minimal draw loop:
*
* @code
*	JMGraphics* gr = new JMGraphics(1920, 1080, "My window");
*	while(gr->windowOpen())
*	{
*	    gr->beginDraw();
*	    //draw stuff here
*	    gr->endDraw();
*	}
*	delete(gr);
* @endcode
*
* Aditional glfwWindowHints may be passed before initialization if reqired.
*/
class JMGraphics {
public:

	///The glfw window object. Be careful accessing this directly.
	GLFWwindow* window = nullptr;
	

	///adds rounded corners to the outlines of 2D shapes if true. slightly impacts performance.
	bool roundedStroke = true;

	///If set to true, buffers will use GL_NEAREST for min and mag filters when created
	bool bufferNearestFilter = false;

	///scales all the graphics including coordinates. if not set to 1.0, 1.0 will not correlate to one pixal. width(), height() and mouse coordinates will also be scaled, but windowWidth() and windowHeight() will not be.
	float scaleFactor = 1.0f;

	float minWindowHeight = 10.0f;
	float maxWindowHeight = 8000.0f;

	float minWindowWidth = 10.0f;
	float maxWindowWidth = 8000.0f;

	///stores all the characters that were typed in the last frame. Resets at the end of each frame.
	std::string typedCharacters = "";

	//------------------------------------------------------------------------------------------------------------------
	
	JMGraphics(int windowWidth, int windowHeight, const char* title);
	JMGraphics(int windowWidth, int windowHeight, const char* title, GLFWmonitor* monitor);
	JMGraphics(int windowWidth, int windowHeight, const char* title, GLFWwindow* parentObject);
	JMGraphics(int windowWidth, int windowHeight, const char* title, GLFWmonitor* monitor, GLFWwindow* parentObject);

	//Delete copy and set move to default
	JMGraphics(const JMGraphics&) = delete;
	JMGraphics& operator=(const JMGraphics&) = delete;
	JMGraphics(JMGraphics&&) = default;
	JMGraphics& operator=(JMGraphics&&) = default;


	~JMGraphics();


	///returns true if the window is open.
	bool windowOpen() const;

	///call once each loop before calling other functions.
	void beginDraw();

	///call once each draw loop at the end of the loop.
	void endDraw();

	

	
	//------------------------------------------------------------------------------------------------------------------
	///has an fbo and a linked texture. Anything drawn between the buffer.beginDraw() and buffer.endDraw() functions is drawn to the buffer instead of the window. 
	class Buffer {
	public:
		GLuint texture = (GLuint)0;
		JMGraphics* parent = nullptr;

		Buffer(float Width, float Height, JMGraphics* parentObject);
		~Buffer();

		//Delete copy and set move to default
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer(Buffer&&) = default;
		Buffer& operator=(Buffer&&) = default;

		void beginDraw();
		void endDraw();
		float width() const;
		float height() const;
		void setSize(float x, float y);
		bool isActive() const;
	private:
		glm::mat4 oldMatrix = glm::mat4(1.0f);
		glm::mat4 texCoordMat = glm::mat4(1.0f);
		GLuint FBO = (GLuint)0;
		GLuint SB = (GLuint)0;
		float Width = 0.0f;
		float Height = 0.0f;
		float scaleFactorLast = 1.0f;

		void setup();

		void createTexture();

		void setTexCoordMat();
		void createTexCoordMat();

		bool isDrawing = false;
		
	};

	///similar to the Buffer object, but for 3D graphics.
	class Buffer3D {
	public:
		GLuint texture = (GLuint)0;
		JMGraphics* parent = nullptr;

		float fov = 0.005f;
		bool perspectiveProjection = true;

		Buffer3D(float Width, float Height, JMGraphics* parentObject);
		~Buffer3D();

		//Delete copy and set move to default
		Buffer3D(const Buffer3D&) = delete;
		Buffer3D& operator=(const Buffer3D&) = delete;
		Buffer3D(Buffer3D&&) = default;
		Buffer3D& operator=(Buffer3D&&) = default;

		void beginDraw();
		void endDraw();
		float width() const;
		float height() const;
		void setSize(float x, float y);
		bool isActive() const;

		///returns the projection matrix used for rendering.
		glm::mat4 projectionMatrix();
		///returns the matrix used to transform NDC to pixel coordinates
		glm::mat4 screenCoordMatrix();

		///defines the closest distance to the camera that will be rendered.
		void clipStart(float value);
		///defines the farthest distance from the camera that will be rendered.
		void clipEnd(float value);

	private:
		GLuint FBO = (GLuint)0;
		GLuint SB = (GLuint)0;
		glm::mat4 oldMatrix = glm::mat4(1.0f);
		glm::mat4 texCoordMat = glm::mat4(1.0f);
		glm::mat4 projMat = glm::mat4(1.0f);
		float Width = 0.0f;
		float Height = 0.0f;
		float scaleFactorLast = 1.0f;


		float ClipStart = 0.1f;
		float ClipEnd = 6000.0f;

		void createTexture();

		void createTexCoordMat();
		void setTexCoordMat();

		void setProjectionMatrix();

		bool isDrawing = false;
	};

	//------------------------------------------------------------------------------------------------------------------
	///object that loads and stores a font.
	class Font {
	public:
		JMGraphics* parent = nullptr;

		///the parent object must be the graphics object that the font will be used in.
		Font(JMGraphics* parentObject);
		~Font();

		//Delete copy and set move to default
		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;
		Font(Font&&) = default;
		Font& operator=(Font&&) = default;


		///loads the font from a file and generates image textures for each character at the specified size.
		bool load(std::string FilePath, int size);

		///renderes the font to the window. Use JMGraphics.setFont, then JMGraphics.text() insdead whenever possible. Only use this if you have created a custom shader program.
		glm::vec2 render(Shader* shader, std::string text, float x, float y, float scale, bool leftJust);
	private:
		int width = 0;
		int height = 0;
		int size = 0;

		float scaleFactorLast = 1.0f;

		///Used to delay re loading the font if the scale factor changes so it doesn't load every frame if it's constantly changing.
		int reLoadCounter = 0;
		bool reLoadNeeded = false;

		std::string filePath;

		typedef struct {
			unsigned int textureID;
			glm::ivec2 size;
			glm::ivec2 bearing;
			unsigned int advance;

		} Character;

		void loadFromFile();
		std::map <char, Character> chars;
	};

	//------------------------------------------------------------------------------------------------------------------
	///Object that loads and stores an image texture.
	class Texture {
	public:
		Texture();
		Texture(std::string FilePath);
		~Texture();

		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		///loads an image texture and saves it to the GPU memory.
		void load(std::string FilePath);
		
		///returns the width of the loaded image in pixels.
		float width() const;
		///returns the height of the loaded image in pixels.
		float height() const;
		///the address of the image texture on the gpu.
		GLuint texture = 0;
		std::string filePath = "";
	private:
		int Width = 0;
		int Height = 0;
		int nChanels = 4;
		unsigned char* data = nullptr;
		
	};

	//------------------------------------------------------------------------------------------------------------------
	///Object that stores an array of lines and can render them much more efficiently than calling line() multiple times.
	class LineArray {
	public:
		LineArray(JMGraphics* Parent);
		~LineArray();

		//Delete copy and set move to default
		LineArray(const LineArray&) = delete;
		LineArray& operator=(const LineArray&) = delete;
		LineArray(LineArray&&) = default;
		LineArray& operator=(LineArray&&) = default;

		bool addLine(glm::vec3 start, glm::vec3 end);
		bool addLine(glm::vec3 start, glm::vec3 end, glm::vec4 color);

		void clearLines();

		void display();

		int totalLines() const;

		JMGraphics* parent = nullptr;
	private:
		int lineNum = 0;
		int VBOsize = 1024;
		std::vector<float> verts;
		unsigned int linesVBO = 0;
		unsigned int linesVAO = 0;
	};

	//------------------------------------------------------------------------------------------------------------------
	///Object that contains and can render mesh data
	class Mesh {
	public:
		Mesh(JMGraphics* Parent);
		~Mesh();

		///Renders the mesh using the parent graphics object.
		void display();
		///Sends the mesh data to the gpu. Must call this after manualy modifying verts if you want it to render correctly.
		///Only call when the parent context or a shared context is current. If not, set the reProcess flag to true
		///and the mesh will be processed the next time display is called.
		void process();

		JMGraphics::Mesh* clone() const;

		//Delete copy and set move to default
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&&) = default;
		Mesh& operator=(Mesh&&) = default;

		struct Vertex
		{
			glm::vec3 pos = glm::vec3(0.0);
			glm::vec3 normal = glm::vec3(0.0f);
			glm::vec2 texCoord = glm::vec2(0.0f);
			glm::vec4 color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		};

		///Writes all vertex and index data to a stream. Stream must be set up correctly for binary writing.
		void writeData(std::ostream& stream);
		///Reads all vertex and index data from a stream and stores them in the given vectors.
		static void readData(std::istream& stream, std::vector<JMGraphics::Mesh::Vertex>& verts, std::vector<unsigned int>& indices);
		///Reads all vertex and index data from a stream.
		void readData(std::istream& stream);

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		std::string name = "Mesh";

		glm::vec4 wireframeColor = glm::vec4(0.0f, 0.3f, 1.0f, 1.0f);

		JMGraphics* parent = nullptr;

		bool renderWireframe = false;
		bool renderFaces = true;
		bool reProcess = false;
	private:	

		unsigned int meshVAO = 0;
		unsigned int meshVBO = 0;
		unsigned int meshEBO = 0;

		unsigned int renderVertNum = 0;
	};

	//------------------------------------------------------------------------------------------------------------------
	///Object that loads, stores and renders meshes.
	class Model {
	public:
		Model(JMGraphics* Parent);
		~Model();

		//Delete copy and set move to default
		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;
		Model(Model&&) = default;
		Model& operator=(Model&&) = default;

		bool load(std::string filePath);
		///Deletes all of the models meshes.
		void clearMeshes();
		///Renders all of the models meshes using the parent graphics object.
		void display();
		void processNodes(aiNode* node, const aiScene* scene);

		void wireframeOn();
		void wireframeOff();
		void facesOn();
		void facesOff();
		void setWireframeColor(glm::vec4 color);
		Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
		JMGraphics::Model* clone() const;

		std::vector<Mesh*> meshes;

		std::string name = "Model";
		
		JMGraphics* parent = nullptr;
	private:
		std::vector<Texture> loadTextures(aiMaterial* material, aiTextureType type);
		std::vector<JMGraphics::Texture> loadedTextures;
		std::string directory = "";
	};


	//------------------------------------------------------------------------------------------------------------------
	///@name Get window properties
	//@{
	
	///returns true if paused at the end of the loop
	bool isPaused() const;
	///returns true if the object is currently in the render loop.
	bool isRendering() const;
	///returns the frame buffer width.
	float width() const;
	///returns the frame buffer height.
	float height() const;
	///returns the window height including any OS elements such as the title bar.
	int windowHeight();
	///returns the window width including any OS elements such as the title bar.
	int windowWidth();
	///returns true if the mouse was moved in the last frame
	bool mouseMoved() const;
	///returns the mouses window position on the x axis.
	float mouseX() const;
	///returns the mouses window position on the x axis.
	float mouseY() const;
	///returns the real mouse position (not taking scale factor into account) on the X axis.
	float realMouseX() const;
	///returns the real mouse position (not taking scale factor into account) on the Y axis.
	float realMouseY() const;
	///returns the mouse position relative to the screen on the x axis
	int globalMouseX() const;
	///returns the mouse position relative to the screen on the x axis
	int globalMouseY() const;
	///returns the frame rate in frames per second, averaged over the last two frames.
	float frameRate() const;
	///returns the amount of time the last frame took in milliseconds.
	float frameTime() const;

	///returns the width of the primary display in pixels.
	int displayWidth() const;
	///returns the height of the primary display in pixels.
	int displayHeight() const;

	///returns the width of a specified display in pixels.
	int displayWidth(int monitor) const;
	///returns the height of a specified display in pixels.
	int displayHeight(int monitor) const;

	///returns the width of a specified display in pixels.
	int displayWidth(GLFWmonitor* monitor) const;
	///returns the height of a specified display in pixels.
	int displayHeight(GLFWmonitor* monitor) const;

	///returns the width of the work area (display size minus task bar and other ui elements) of the primary display.
	int workAreaWidth() const;
	///returns the height of the work area (display size minus task bar and other ui elements) of the primary display.
	int workAreaHeight() const;

	///returns the width of the work area (display size minus task bar and other ui elements) of a specified display.
	int workAreaWidth(int monitor) const;
	///returns the height of the work area (display size minus task bar and other ui elements) of a specified display.
	int workAreaHeight(int monitor) const;

	///returns the width of the work area (display size minus task bar and other ui elements) of a specified display.
	int workAreaWidth(GLFWmonitor* monitor) const;
	///returns the wheightidth of the work area (display size minus task bar and other ui elements) of a specified display.
	int workAreaHeight(GLFWmonitor* monitor) const;

	///returns true if the window was resized from the last frame
	bool windowResized() const;

	///returns the number of monitors that are available.
	int monitorNum() const;

	bool scaleFactorChanged() const;

	///returns the current transformation matrix. The matrix is transformed in pixel coordinates, not NDC.
	glm::mat4 transformationMatrix() const;

	///returns the matrix used to convert from pixel coordinates to NDC.
	glm::mat4 screenCoordMatrix() const;

	//@}

	//------------------------------------------------------------------------------------------------------------------
	///@name Set window properties
	//@{
	
	///pauses at the end of the draw loop in a safe condidion to use draw funcitons from a different thread.
	///Will not pause immediately!
	///If called multiple times, resume() will have to be called the same number of times to resume.
	///Call forceResume() to resume emediately even if paused multiple times.
	void pauseInLoop();
	///Resume the draw loop if paused. Needs to be called the same number of times pause() has been called before resuming.
	///Call forceResume() to resume immediately event if paused multiple times.
	void resume();
	///Forces the thread to resume even if pause was called multiple times
	void forceResume();
	///sets the window size. Returns true if successfull.
	bool size(int x, int y) const;
	///sets the window position on the display
	void windowPosition(int x, int y) const;
	///maximizes the window
	void maximize() const;
	///minimizes the window
	void minimize() const;
	///sets the window to fullscreen
	void fullscreen();
	///makes the window fullscreen on a specific monitor
	void fullscreen(int monitor);
	///makes the window fullscreen on a specific monitor
	void fullscreen(GLFWmonitor* monitor) const;
	///makes the window fullscreen on a specific monitor with a specific resolution
	void fullscreen(GLFWmonitor* monitor, int width, int height) const;
	///define if the user can resize the monitor
	void isResizeable(bool resizable) const;
	///applies an upper frame rate limit. set to zero for unlimited frame rate.
	void limitFrameRate(int rate);
	///Sets the window to close at the end of this draw loop.
	void closeWindow();
	//@}

	//------------------------------------------------------------------------------------------------------------------
	///@name Set shape drawing properties
	//@{

	///sets the color used to fill in shaapes.  inputs need to be between 0.0 and 1.0.
	void fill(float r, float g, float b, float a);
	///sets the color used to outline shapes.  inputs need to be between 0.0 and 1.0.
	void stroke(float r, float g, float b, float a);

	///sets the color used to fill in shaapes.  inputs need to be between 0.0 and 1.0.
	void fill(glm::vec4 color);
	///sets the color used to fill in shaapes.  inputs need to be between 0.0 and 1.0.
	void fill(glm::vec3 color);
	///sets the color used to outline shapes.  inputs need to be between 0.0 and 1.0.
	void stroke(glm::vec4 color);
	///sets the color used to outline shapes.  inputs need to be between 0.0 and 1.0.
	void stroke(glm::vec3 color);

	///sets the color used to fill in shapes.  inputs need to be between 0.0 and 1.0.
	void fill(float r, float g, float b);
	///sets the color used to outline shapes.  inputs need to be between 0.0 and 1.0.
	void stroke(float r, float g, float b);

	///sets the color used to fill in shapes.  inputs need to be between 0.0 and 1.0.
	void fill(float v);
	///sets the color used to outline shapes.  inputs need to be between 0.0 and 1.0.
	void stroke(float v);

	///returns a vector storing the rgba values of a color interpolated from colorA to colorB by the factor parameter. factor must be between 0.0f and 1.0f.
	static glm::vec4 lerpColor(glm::vec4 colorA, glm::vec4 colorB, float factor);
	///returns a vector storing the rgba values of a color interpolated from colorA to colorB to colorC by the factor parameter. factor must be between 0.0f and 1.0f.
	static glm::vec4 lerpColor(glm::vec4 colorA, glm::vec4 colorB, glm::vec4 colorC, float factor);
	///returns the inverse of the input color. Alpha will be the same as the input color.
	static glm::vec4 invertColor(glm::vec4 color);
	///Returns a string of a value without trailing zeros
	static std::string cleanValString(float val);
	///Returns a string of a value without trailing zeros
	static std::string cleanValString(double val);

	///sets the width of the outlines on shapes
	void strokeWeight(float weight);
	///shapes will not be filled in
	void noFill();
	///shapes will have no outline
	void noStroke();

	///sets which font to use when drawing text
	void setFont(Font* font);
	///scales the font. 1.0 makes it equal to the scale the font was loaded in at. if a font is loaded at a small size and scaled up, quality will be bad.
	void textSize(float size);

	///fills the whole window with a background of a specified color.  inputs need to be between 0.0 and 1.0.
	void background(float r, float g, float b, float a);
	///fills the whole window with a background of a specified color.  inputs need to be between 0.0 and 1.0.
	void background(float r, float g, float b);
	///fills the whole window with a background of a specified color.  inputs need to be between 0.0 and 1.0.
	void background(float v);

	///sets whether or not to use FXAA (fast approx anti-aliasing) when rendering textures amd graphics buffers.
	void textureFxaa(bool FXAAon);
	//@}

	//------------------------------------------------------------------------------------------------------------------
	///@name Matrix transformation
	//@{

	///pushes the transfromation matrix to the stack
	void pushMatrix();
	///pops the transformation matrix from the stack
	void popMatrix();
	///resets the transformation matrix
	void resetMatrix();
	///Multiplies the transformation matrix by another matrix
	void multiplyMatrix(glm::mat4 matrix);
	///applies 2d rotation to the transformation matrix
	void rotate(float angle);
	///applies 3d rotation to the transformation matrix in the form of Euler angles.
	void rotate(float X, float Y, float Z);
	///applies 3d rotation to the transformation matrix in the form of a rotation axis and angle.
	void rotate(float angle, glm::vec3 axis);
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
	///applies translation to the transformation matrix
	void translate(float x, float y, float z);
	///applies translation to the transformation matrix
	void translate(float x, float y);
	///applies scale to the transformation matrix
	void scale(float x, float y, float z);
	///applies scale to the transformation matrix
	void scale(float x, float y);
	//@}

	//------------------------------------------------------------------------------------------------------------------
	///@name shape drawing functions
	//@{
	
	///draws a rectangle centered on the posX and posY coordinates.
	void rect(float posX, float posY, float sizeX, float sizeY);
	///draws a rectangle with rounded corners centered on the x and y coordinates. radius specifies the radius of the corners.
	void rect(float x, float y, float sizeX, float sizeY, float radius);

	///draws a rectangle filled with a gradient. the first color parameter is the left side, the second is the right
	void gradient(float posX, float posY, float sizeX, float sizeY, glm::vec4 color1, glm::vec4 color2);
	///draws a rectangle filled with a gradient. the first color parameter is the left side, the second is the right
	void gradient(float posX, float posY, float sizeX, float sizeY, glm::vec3 color1, glm::vec3 color2);
	///draws a rectangle filled with a gradient. the first color parameter is the left side, the second is the right
	void gradient(float posX, float posY, float sizeX, float sizeY, float color1, float color2);
	
	///draws an ellipse centered on the x and y coordinates.
	void ellipse(float x, float y, float sizeX, float sizeY);
	///draws a circle centered on the x and y coordinates.
	void ellipse(float x, float y, float diameter);
	
	///draws a symetrical polygin with a specified number of sides, centered on the x and y coordinates.
	void polygon(float x, float y, float sizeX, float sizeY, int sides);
	///draws a symetrical polygin with a specified number of sides, centered on the x and y coordinates.
	void polygon(float x, float y, float radius, int sides);

	///draws a line between two 2D points.
	void line(float x1, float y1, float x2, float y2);
	///draw a line between two 3D points.
	void line(float x1, float y1, float z1, float x2, float y2, float z2);
	///draws a dashed line between two 2D points with specified dash length and gap length.
	void dashedLine(float x1, float y1, float x2, float y2, float dashLength, float gapLength);
	///draws a dashed line between two 3D points with specified dash length and gap length.
	void dashedLine(float x1, float y1, float z1, float x2, float y2, float z2, float dashLength, float gapLength);
	///draws a point at a specified position.
	void point(float x, float y);

	///draws a 3D box with specified sizes on the x, y and z axis. Draws centered at coordinates 0, 0, 0 so use translate to set position.
	void box(float x, float y, float z);
	///draws a cube with a specified side length.  Draws centered at coordinates 0, 0, 0 so use translate to set position.
	void cube(float size);
	
	///draws an arc as a portion of a circle centered on the x and y coordinates. pie boolean sets weather or not to draw the outlines on the inner edges of the arc ends.
	void arc(float x, float y, float sizeX, float sizeY, float startAngle, float endAngle, bool pie);
	///draws an arc as a portion of a circle centered on the x and y coordinates.
	void arc(float x, float y, float sizeX, float sizeY, float startAngle, float endAngle);

	///draws the contents of a buffer object centered on the x and y coordinates scaled to a specified x and y size.
	void image(Buffer* buffer, float x, float y, float sizeX, float sizeY);
	///draws the contents of a buffer object centered on the x and y coordinates.
	void image(Buffer* buffer, float x, float y);
	///draws the contents of a buffer object centered on the x and y coordinates scaled to a specified x and y size.
	void image(Buffer3D* buffer, float x, float y, float sizeX, float sizeY);
	///draws the contents of a buffer object centered on the x and y coordinates.
	void image(Buffer3D* buffer, float x, float y);
	///draws an image texture centered on the x and y coordinates scaled to a specified x and y size.
	void image(Texture* texture, float x, float y, float sizeX, float sizeY);
	///draws an image texture centered on the x and y coordinates.
	void image(Texture* texture, float x, float y);
	///draws an opengl texture to the screen on a quad.
	void displayTexture(GLuint texture, float x, float y, float sizeX, float sizeY);

	///draws a string of text to the window. Must load and set a font first!
	void text(std::string text, float posX, float posY);
	///draws a string of text to the window. The size of the text is stored in the textSize vector.  Must load and set a font first!
	void text(std::string text, float posX, float posY, glm::vec2 &textSize);

	///draws a string of text to the window. Must load and set a font first!
	void text(std::string text, float posX, float posY, bool leftJustify);
	///draws a string of text to the window. The size of the text is stored in the textSize vector.  Must load and set a font first!
	void text(std::string text, float posX, float posY, bool leftJustify, glm::vec2 &textSize);
	//@}

	//------------------------------------------------------------------------------------------------------------------
	///@name User input
	//@{

	///returns true if the left mouse button is currently pressed
	bool mousePressed() const;
	///returns true if the right mouse button is currently pressed
	bool mousePressedRight() const;
	///returns true if the left mouse button was pressed and released
	bool mouseClicked() const;
	///returns true if the left mouse button was double clicked
	bool mouseDoubleClicked() const;
	///returns true if the right mouse button was double clicked
	bool mouseRightDoubleClicked() const;
	///returns true if the right mouse button was pressed and released
	bool mouseClickedRight() const;
	///returns true if the left mouse button was pressed on the last frame
	bool mousePressedLast() const;
	///returns true if the right mouse button was pressed on the last frame
	bool mouseRightPressedLast() const;
	///returns the amount scrolled in the x direction with the mouse wheel or touchpad during the last frame
	float scrollX() const;
	///returns the amount scrolled in the y direction with the mouse wheel or touchpad during the last frame
	float scrollY() const;
	///returns true if the mouse wheel was scrolled during the last frame
	bool ScrolledLast() const;

	///returns true if the input key is currently pressed. works while multiple keys are pressed at the same time.
	bool keyDown(int key) const;

	///returns true if the input key was pressed in the last frame
	bool keyPressed(int key) const;
	///returns true if the input key was released in the last frame
	bool keyReleased(int key) const;

	///Returns true if either of the control keys are pressed
	bool controlKeyDown() const;
	///Returns true if either of the alt keys are pressed
	bool altKeyDown() const;
	///Returns true if either of the shift keys are pressed
	bool shiftKeyDown() const;

	//this is passed to glfwSetKeyCallback. keyboard input is then handled by on_key_callback.
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		/// Access class instance from user pointer
		JMGraphics* gr = static_cast<JMGraphics*>(glfwGetWindowUserPointer(window));
		
		/// Call member function
		if (gr) {
			gr->on_key_callback(window, key, scancode, action, mods);
		}
	}
	void on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void character_callback(GLFWwindow* window, unsigned int codepoint) {
		JMGraphics* gr = static_cast<JMGraphics*>(glfwGetWindowUserPointer(window));

		if (gr) {
			gr->on_character_callback(window, codepoint);
		}
	}

	void on_character_callback(GLFWwindow* window, unsigned int codepoint);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		JMGraphics* gr = static_cast<JMGraphics*>(glfwGetWindowUserPointer(window));

		if (gr) {
			gr->on_framebuffer_size_callback(window, width, height);
		}
	}

	void on_framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		JMGraphics* gr = static_cast<JMGraphics*>(glfwGetWindowUserPointer(window));

		if (gr) {
			gr->on_scroll_callback(window, xoffset, yoffset);
		}
	}

	void on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	//@}

	///------------------------------------------------------------------------------------------------------------------
	///@name Other
	//@{

	///smoothly increases the input value to 1.0f taking the frame rate into account.
	float smoothIncrease(float input, float scaler) const;
	///smoothly decreases the input value to 0.0f taking the frame rate into account.
	float smoothDecrease(float input, float scaler) const;
	///linearly increases the input value to 1.0f taking the frame rate into account.
	float linearIncrease(float input, float scaler) const;
	///linearly decreases the input value to 0.0f taking the frame rate into account.
	float linearDecrease(float input, float scaler) const;

	//@}

private:

	FT_Library freeTypeLib = NULL;

	bool scrolledLast = false;

	//used to lock this thread while paused.
	std::mutex privMtx;
	//used to lock this thread while paused.
	std::condition_variable privPauseCondition;

	bool IsPaused = false;
	//keeps track of weather the object is rendering. Usefull for multithreaded applications.
	bool IsRendering = false;

	unsigned int PauseNum = 0;

	//transfromation matrices
	glm::mat4 tranformMat = glm::mat4(1.0f);
	glm::mat4 screenCoordMat = glm::mat4(1.0f);
	std::stack<glm::mat4> transMat2DStack;

	//stores the mouse button states last frame
	bool mousePressedLeftLast = false;
	bool mousePressedRightLast = false;
	bool MouseLeftDoubleCLicked = false;
	bool MouseRightDoubleClicked = false;

	//timer used to detect double clicks
	float mouseLeftTimer = 1000.0f;
	float mouseRightTimer = 1000.0f;
	//the amount of time after the first click to count as a double click in ms
	float doubleClickTime = 300.0f;

	bool WindowResized = false;
	bool ScaleFactorChanged = false;

	//stores the number of mouse wheel ticks last frame
	float ScrollX = 0;
	float ScrollY = 0;

	//VAO = vertex array obj, VBO = vertex buffer obj, EBO = inceces buffer obj.
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	//used to detect if the scale factor has changed
	float scaleFactorLast = 1.0f;

	//empty arrays used to initialize the buffers
	const static int initVertNum = 128 * 7;
	const static int initIndexNum = 128 * 3 + 1;
	float initVertices[initVertNum];
	unsigned int initIndices[initIndexNum];

	//keeps track of the size of the data in the buffers, in order to re-alocate only when needed.
	GLsizei VBOsize = 0;
	GLsizei EBOsize = 0;

	//shader objects
	Shader* shaderColor = nullptr;
	Shader* shaderTex = nullptr;
	Shader* shader2DFont = nullptr;
	Shader* shaderMesh = nullptr;

	//Activates a shader if not already active.
	void activateShader(Shader* shader);

	//A pointer to the last activated shader. Used to avoid activating the same shader repeatedly.
	Shader* activeShader = nullptr;

	//re writes the vao data with new shape data. used in each shape function
	void overwriteVBO(float vertices[], GLsizei arraySize);
	void overwriteEBO(unsigned int indices[], GLsizei arraySize);

	//sets up the vertex atrib pointers assuming that the vertices have 7 values X, Y, Z, and R, G, B, A
	void setAtribPoiner_default();

	//stores the viewport size in pixels
	int Width = 0;
	int Height = 0;
	//used to check if the window was resized
	int widthLast = 0;
	int heightLast = 0;
	//stores the window size in pixels;
	int WindowWidth = 0;
	int WindowHeight = 0;
	//the mouse curser location
	double MouseX = 0;
	double MouseY = 0;
	double MouseXLast = 0;
	double MouseYLast = 0;

	//the maximum frame time to allow
	float minFrameTimeTarget = 0.0f;
	float minFrameTime = 0.0f;

	//the frame rate and time of each frame
	float FrameRate = 60.0f;
	float FrameTime = 15.0f;
	float FrameStart = 0.0f;

	//stores the monitor info for all the monitors
	GLFWmonitor** monitors = nullptr;
	int monitorCount = 1;

	//properties for the shape drawing functions
	//stores the colors to fill shapes
	glm::vec4 fillColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 strokeColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	float strokeWeightValue = 1.0f;
	//weather or not to fill or outline shapes
	bool useFill = true;
	bool useStroke = true;

	//the size to draw text to the screen
	float TextSize = 1.0f;

	//the current font used to draw text
	Font* currentFont = nullptr;

	//stores all the currently pressed keys
	int pressedKeys[10];

	//stores the last pressed key for one frame
	int pressedKey = 0;
	//stores the released key for one frame
	int releasedKey = 0;

	//used by the shape functions to override the color info imbeded in the vertex data.
	GLint shader2DColorOverride = (GLint)0;
	GLint shader2DColor = (GLint)0;

	void setupWindow(int windowWidth, int windowHeight, const char* title, GLFWmonitor* monitor, GLFWwindow* parentObject);

	//creates a matrix to scale/offset opengl coordinates by the window size in pixels.
	void setScreenCoordMat();

	//processes user input from keyboard and mouse.
	void processInput(GLFWwindow* window);
};