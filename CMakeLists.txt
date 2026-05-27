find_package(OpenGL REQUIRED)


set(GLAD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Linking/lib/glad/glad.c")

set_source_files_properties(${GLAD_SRC} PROPERTIES LANGUAGE C)

add_library(glad STATIC ${GLAD_SRC})
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Linking/include)

set_target_properties(glad PROPERTIES
    LINKER_LANGUAGE C
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/debug
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/release
)

add_library(JMGraphics
    src/JMG_drawingProperties.cpp
    src/JMG_fontAndText.cpp
    src/JMG_ioFunctions.cpp
    src/JMG_LineArray.cpp
    src/JMG_Mesh.cpp
    src/JMG_Model.cpp
    src/JMG_shader.cpp
    src/JMG_shapeFunctions.cpp
    src/JMG_texturesAndBuffers.cpp
    src/JMG_transformFunctions.cpp
    src/JMG_windowManagement.cpp
    src/JMGraphics.cpp
    src/stb.cpp
    JMGraphics.h
    JMG_shader.h
)

target_include_directories(JMGraphics
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Linking/include
)

target_link_libraries(JMGraphics
    PUBLIC
        glad
        OpenGL::GL

        ${CMAKE_CURRENT_SOURCE_DIR}/Linking/lib/assimp/assimp-vc143-mt.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/Linking/lib/glfw/glfw3_mt.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/Linking/lib/freetype/freetype.lib
)

if (MSVC)
    target_compile_options(JMGraphics PRIVATE /bigobj)
endif()