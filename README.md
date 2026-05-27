# JMGraphics

An OpenGL based graphics engine that can draw various shapes, meshes, text and images.

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

# CMake setup
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

