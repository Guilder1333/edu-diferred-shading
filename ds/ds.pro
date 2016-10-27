TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Cube.cpp \
    Mesh.cpp \
    Object.cpp \
    Renderable.cpp \
    Triangle.cpp \
    RenderPlane.cpp \
    Logger.cpp \
    Texture.cpp \
    VertexShader.cpp \
    Shader.cpp \
    Camera.cpp \
    FragmentShader.cpp \
    ShaderProgram.cpp \
    RenderScene.cpp \
    Material.cpp \
    ProgramVariable.cpp \
    ResourceManager.cpp \
    Light.cpp \
    Window.cpp

DISTFILES += \
    ForwardFragmentShader.glsl \
    ForwardVertexShader.glsl \
    ShadowVertexShader.glsl \
    ShadowFragmentShader.glsl \
    DSFPVertexShader.glsl \
    DSFPFragmentShader.glsl \
    DSSPFragmentShader.glsl \
    DSSPVertexShader.glsl

HEADERS += \
    Cube.h \
    Mesh.h \
    Object.h \
    Renderable.h \
    Triangle.h \
    RenderPlane.h \
    Logger.h \
    Texture.h \
    VertexShader.h \
    Shader.h \
    Camera.h \
    FragmentShader.h \
    ShaderProgram.h \
    RenderScene.h \
    Material.h \
    ProgramVariable.h \
    ResourceManager.h \
    Light.h \
    Window.h

win32: LIBS += \
    -L"C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64" -lWinMM -lVersion -lImm32 -lOpenGL32 -lUser32 -lShell32 -lOle32 -lGdi32 -lOleAut32 \
    -L$$PWD/3rdParty/glew/lib/ -lglew32 \
    -L$$PWD/3rdParty/sdl/lib/ -lSDL2 -lSDL2main \
    -L$$PWD/3rdParty/lib3ds/lib -llib3ds-101d \
    -L$$PWD/3rdParty/freeImage/lib -lFreeImage

INCLUDEPATH += \
    $$PWD/3rdParty/glew/include \
    $$PWD/3rdParty/sdl/include \
    $$PWD/3rdParty/lib3ds/include \
    $$PWD/3rdParty/freeImage/include \
    $$PWD/3rdParty/glm/include \
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10150.0/ucrt"

DEPENDPATH +=

#shaders.files = *.glsl
#shaders.path = $$DESTDIR
#INSTALLS += shaders
