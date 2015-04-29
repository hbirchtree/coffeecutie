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
    opengl/coffeescene.cpp \
    general/models/wavefrontmodelreader.cpp \
    general/models/modelreaderinterface.cpp \
    opengl/helpers/renderingmethods.cpp \
    opengl/helpers/texturehelper.cpp \
    opengl/components/coffeetexture.cpp \
    opengl/components/coffeecamera.cpp \
    general/physics/physicsobject.cpp \
    opengl/components/coffeeworldopts.cpp \
    opengl/components/coffeeomnilight.cpp \
    general/coffeejoystick.cpp

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
    general/physics/physicsobject.h \
    opengl/components/coffeematerial.h \
    general/models/wavefrontmodelreader.h \
    general/models/modelreaderinterface.h \
    opengl/helpers/renderingmethods.h \
    opengl/components/coffeetexture.h \
    opengl/components/coffeecamera.h \
    opengl/helpers/coffeegameasset.h \
    general/common.h \
    opengl/components/coffeeworldopts.h \
    opengl/components/coffeeomnilight.h \
    general/coffeejoystick.h

#INCLUDEPATH += $$PWD/../glfw-library/src/
#DEPENDPATH += $$PWD/../glfw-library/src/

#unix:!macx: PRE_TARGETDEPS += $$PWD/../glfw-library/src/libglfw3.a

DISTFILES += \
    include/glm/CMakeLists.txt

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/glfw-build/src/release/ -lGL -lGLEW -lglfw3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/glfw-build/src/debug/ -lGL -lGLEW -lglfw3
else:unix: LIBS += -L$$PWD/libs/glfw-build/src/ -lGL -lGLEW -lglfw3 -lX11 -lXxf86vm -lXinerama -lXcursor -lXrandr -lpthread -lXi

INCLUDEPATH += $$PWD/libs/glfw/include
DEPENDPATH += $$PWD/libs/glfw/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/glfw-build/src/release/libglfw3.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/glfw-build/src/debug/libglfw3.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/glfw-build/src/release/glfw3.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/glfw-build/src/debug/glfw3.lib
else:unix: PRE_TARGETDEPS += $$PWD/libs/glfw-build/src/libglfw3.a
