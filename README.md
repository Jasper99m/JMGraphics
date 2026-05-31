# JMGraphics

A simple graphics engine that can be used to generate interactive 2D or 3D graphics.

Each instance of the JMGraphics class opens one window. Each frame must start by calling beginDraw()
and end by calling endDraw().

Example of a minimal draw loop:

```
	JMGraphics* gr = new JMGraphics(1920, 1080, "My window");
	while(gr->windowOpen())
	{
	    gr->beginDraw();
	    //draw stuff here
	    gr->endDraw();
	}
	delete(gr);
```

Aditional glfwWindowHints may be passed before initialization if reqired.


To render a simple shape in your draw loop, first set fill and stroke colors as well as the stroke weight,
then call a function to draw your desired shape
```
	//Sets the background to black
	gr->background(0.0f);
	
	//Colors are set as v, rgb or rgba and values should be between 0.0f and 1.0f
	gr->fill(0.2f, 0.6f, 1.0f, 1.0f);
	gr->stroke(1.0f, 1.0f, 1.0f, 1.0f);

	//Sets the width of the lines in pixels
	gr->strokeWeight(1.0f);

	//Draws a rectangle centered in the middle of the window with a width and height of 50 pixels
	gr->rect(gr->width() / 2.0f, gr->height() / 2.0f, 50.0f, 50.0f);
```

By default, 2D graphics can be rendered directly to the window or to a Buffer instance,
while 3D graphics must be rendered to a Buffer3D instance.  Render to a buffer by calling the drawing functions
between the buffers BeginDraw() and EndDraw() functions.  Buffer objects can then be displayed
in the window using Image().

# Setup
A good way to include this library is to add this repo as a git submodule in your project.

This library is designed to be built using CMake and has its own CMakeLists.txt.
To set up your own CMakeLists.txt to include this library, you must add
```
add_subdirectory(JMGraphics)

target_link_libraries(YourProjectName
   PUBLIC JMGraphics
)
```
And to copy assets to the build folder
```
add_custom_command(TARGET YourProjectName POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/JMGraphics/assimp-vc143-mt.dll $<TARGET_FILE_DIR:YourProjectName>
COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/JMGraphics/shaders $<TARGET_FILE_DIR:YourProjectName>/JMGraphics/shaders
)
```

If you want your project to statically link MSVC in order to be run as a portable without installing anything,
Put this before linking the JMGraphics library in your CMakeLists.txt.
```
if (MSVC)
   set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
```

