#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T17:35:23
#
#-------------------------------------------------

QT       += core gui
#QtGUI is used for image loading

TARGET = CoffeeCutie
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/include
CONFIG += c++11

SOURCES += main.cpp \
    opengl/coffeerenderer.cpp \
    general/vectors/vector3container.cpp \
    general/floatcontainer.cpp \
    opengl/helpers/shadercontainer.cpp \
    opengl/coffeescene.cpp

HEADERS += \
    opengl/coffeerenderer.h \
    opengl/rendering/renderableobject.h \
    general/vectors/vector3container.h \
    general/floatcontainer.h \
    opengl/helpers/shadercontainer.h \
    opengl/helpers/texturehelper.h \
    opengl/helpers/vaohelper.h \
    general/filehandler.h \
    opengl/rendering/coffeevertex.h \
    opengl/rendering/coffeemesh.h \
    opengl/coffeescene.h \
    opengl/components/coffeeobject.h \
    general/physics/physicsobject.h

unix:!macx: LIBS += -L$$PWD/../glfw-library/src/ -lGL -lGLEW -lglfw3 -lX11 -lXxf86vm -lXinerama -lXcursor -lXrandr -lpthread -lXi

INCLUDEPATH += $$PWD/../glfw-library/src/
DEPENDPATH += $$PWD/../glfw-library/src/

unix:!macx: PRE_TARGETDEPS += $$PWD/../glfw-library/src/libglfw3.a

DISTFILES += \
    include/glm/CMakeLists.txt
