#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T17:35:23
#
#-------------------------------------------------

QT       += core gui opengl
#QtGUI is used for image loading

TARGET = CoffeeCutie
CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/include

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
    opengl/helpers/vaohelper.cpp \
    opengl/rendering/coffeemesh.cpp \
    opengl/rendering/coffeevertex.cpp \
    general/input/coffeesdljoystick.cpp \
    general/input/coffeejoystick.cpp \
    opengl/rendering/coffeerenderingloop.cpp \
    tests/boxtest.cpp \
    tests/CubeScape.cpp \
    tests/glutils.cpp \
    tests/RawFile.cpp \
    opengl/qcoffeerenderer.cpp \
    opengl/qcoffeewindow.cpp

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
    general/input/coffeesdljoystick.h \
    general/input/coffeejoystick.h \
    opengl/rendering/coffeerenderingloop.h \
    tests/boxtest.h \
    tests/CubeScape.h \
    tests/glutils.h \
    tests/RawFile.h \
    opengl/rendering/renderloop.h \
    opengl/qcoffeerenderer.h \
    opengl/qcoffeewindow.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../glbinding-library/release/ -lglbinding
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../glbinding-library/debug/ -lglbinding
else:unix: LIBS += -L$$PWD/../glbinding-library/ -lglbinding

INCLUDEPATH += $$PWD/libs/glbinding/source/glbinding/include
DEPENDPATH += $$PWD/libs/glbinding/source/glbinding/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../glfw-library/src/release/ -lGL -lGLEW -lglfw3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../glfw-library/src/debug/ -lGL -lGLEW -lglfw3
else:unix: LIBS += -L$$PWD/../glfw-library/src/ -lGL -lglfw3 -lX11 -lXxf86vm -lXinerama -lXcursor -lXrandr -lpthread -lXi

INCLUDEPATH += $$PWD/libs/glfw/include
DEPENDPATH += $$PWD/libs/glfw/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../glfw-library/src/release/libglfw3.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../glfw-library/src/debug/libglfw3.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../glfw-library/src/release/glfw3.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../glfw-library/src/debug/glfw3.lib
else:unix: PRE_TARGETDEPS += $$PWD/../glfw-library/src/libglfw3.a
