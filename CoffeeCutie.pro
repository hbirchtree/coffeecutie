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
    engine/models/coffeemesh.cpp \
    engine/objects/coffeeobjectfactory.cpp \
    engine/objects/coffeesimpleobject.cpp \
    engine/physics/physicsobject.cpp \
    engine/rendering/coffeeadvancedloop.cpp \
    engine/rendering/coffeerenderingmethod.cpp \
    general/data/coffeedatacontainer.cpp \
    general/data/mutabledatacontainer.cpp \
    general/data/numbercontainer.cpp \
    general/input/coffeejoystick.cpp \
    general/input/coffeeplayercontroller.cpp \
    general/input/coffeesdljoystick.cpp \
    general/models/modelreaderinterface.cpp \
    general/models/wavefrontmodelreader.cpp \
    general/qstringfunctions.cpp \
    general/shadervariant.cpp \
    opengl/components/coffeecamera.cpp \
    opengl/components/coffeeframebufferobject.cpp \
    opengl/components/coffeeomnilight.cpp \
    opengl/components/coffeetexture.cpp \
    opengl/components/coffeeworldopts.cpp \
    opengl/components/shadercontainer.cpp \
    opengl/context/coffeerenderer.cpp \
    opengl/context/qcoffeerenderer.cpp \
    opengl/context/qcoffeewindow.cpp \
    opengl/helpers/renderingmethods.cpp \
    opengl/helpers/texturehelper.cpp \
    opengl/helpers/vaohelper.cpp \
    opengl/rendering/coffeerenderingloop.cpp \
    opengl/rendering/coffeevertex.cpp \
    tests/boxtest.cpp \
    tests/CubeScape.cpp \
    tests/glutils.cpp \
    tests/RawFile.cpp \
    coffeelogger.cpp \
    engine/physics/bulletphysics.cpp \
    engine/objects/coffeeoutputsurface.cpp \
    opengl/components/coffeedepthbufferobject.cpp \
    engine/physics/physicsdescriptor.cpp \
    inspector/coffeeinspector.cpp \
    engine/objects/coffeeparticlesystem.cpp

HEADERS += \
    engine/models/coffeemesh.h \
    engine/objects/coffeeobjectfactory.h \
    engine/objects/coffeesimpleobject.h \
    engine/physics/physicsobject.h \
    engine/rendering/coffeeadvancedloop.h \
    engine/rendering/coffeerenderingmethod.h \
    general/data/coffeedatacontainer.h \
    general/data/coffeegameasset.h \
    general/data/mutabledatacontainer.h \
    general/data/numberbuffer.h \
    general/data/numbercontainer.h \
    general/input/coffeejoystick.h \
    general/input/coffeeplayercontroller.h \
    general/input/coffeesdljoystick.h \
    general/models/modelreaderinterface.h \
    general/models/wavefrontmodelreader.h \
    general/common.h \
    general/filehandler.h \
    general/qstringfunctions.h \
    general/shadervariant.h \
    opengl/components/coffeecamera.h \
    opengl/components/coffeeframebufferobject.h \
    opengl/components/coffeematerial.h \
    opengl/components/coffeeobject.h \
    opengl/components/coffeeomnilight.h \
    opengl/components/coffeetexture.h \
    opengl/components/coffeeworldopts.h \
    opengl/components/shadercontainer.h \
    opengl/context/coffeerenderer.h \
    opengl/context/qcoffeerenderer.h \
    opengl/context/qcoffeewindow.h \
    opengl/helpers/renderingmethods.h \
    opengl/helpers/texturehelper.h \
    opengl/helpers/vaohelper.h \
    opengl/rendering/coffeerenderingloop.h \
    opengl/rendering/coffeevertex.h \
    opengl/rendering/renderableobject.h \
    opengl/rendering/renderloop.h \
    tests/boxtest.h \
    tests/CubeScape.h \
    tests/glutils.h \
    tests/RawFile.h \
    coffeelogger.h \
    engine/physics/genericphysicsinterface.h \
    engine/physics/bulletphysics.h \
    engine/objects/coffeeoutputsurface.h \
    opengl/components/coffeedepthbufferobject.h \
    engine/physics/physicsdescriptor.h \
    inspector/coffeeinspector.h \
    engine/objects/coffeeparticlesystem.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bullet-library/src/release/ -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bullet-library/src/debug/ -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
else:unix: LIBS += -L$$PWD/../bullet-library/src/ -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath

INCLUDEPATH += $$PWD/libs/bullet/src
DEPENDPATH += $$PWD/libs/bullet/src

FORMS += \
    inspector/coffeeinspector.ui
