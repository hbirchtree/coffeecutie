#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T17:35:23
#
#-------------------------------------------------

QT       += core gui opengl concurrent script

TARGET = CoffeeCutie
CONFIG += console c++11
CONFIG -= app_bundle

#DEFINES += QOPENGL_CONTEXT_MANAGER

TEMPLATE = app

INCLUDEPATH += $$PWD/include

SOURCES += main.cpp \
    engine/models/coffeemesh.cpp \
    engine/objects/coffeeobjectfactory.cpp \
    engine/physics/physicsobject.cpp \
    engine/rendering/coffeeadvancedloop.cpp \
    general/input/coffeejoystick.cpp \
    general/input/coffeeplayercontroller.cpp \
    general/qstringfunctions.cpp \
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
    tests/boxtest.cpp \
    tests/CubeScape.cpp \
    tests/glutils.cpp \
    tests/RawFile.cpp \
    coffeelogger.cpp \
    engine/physics/bulletphysics.cpp \
    engine/objects/coffeeoutputsurface.cpp \
    engine/physics/physicsdescriptor.cpp \
    inspector/coffeeinspector.cpp \
    engine/objects/coffeeparticlesystem.cpp \
    inspector/coffeerendererinspector.cpp \
    inspector/graphplotter.cpp \
    engine/objects/coffeeobject.cpp \
    engine/objects/coffeestandardobject.cpp \
    engine/objects/coffeeskybox.cpp \
    opengl/components/coffeematerial.cpp \
    engine/models/coffeeinstancecontainer.cpp \
    general/filehandler.cpp \
    engine/data/coffeeassetimporter.cpp \
    inspector/coffeescriptterminal.cpp \
    engine/scripting/qscriptvectorvalue.cpp \
    engine/scripting/coffeescriptengine.cpp \
    engine/physics/genericphysicsinterface.cpp \
    engine/scripting/coffeeinputevent.cpp \
    opengl/components/framebuffers/coffeeframebufferbaseclass.cpp \
    engine/data/coffeeresource.cpp \
    engine/ai/coffeeneuralnet.cpp \
    engine/models/coffeeskeleton.cpp \
    engine/compute/coffeecomputeresource.cpp \
    engine/compute/coffeetransformcomputer.cpp \
    opengl/helpers/coffeeperlinnoisesource.cpp \
    engine/interface/coffeedrawableelement.cpp \
    engine/shaders/coffeeuniformsetter.cpp \
    opengl/components/coffeebuffer.cpp \
    engine/models/coffeevertexarrayobject.cpp \
    opengl/components/coffeeuniformblock.cpp \
    engine/rendering/coffeerendergraph.cpp \
    engine/data/vectors/matrix4value.cpp \
    engine/data/vectors/vector3value.cpp \
    engine/data/vectors/quatvalue.cpp \
    engine/data/vectors/scalarvalue.cpp \
    engine/data/vectors/vector2value.cpp \
    engine/data/vectors/vector4value.cpp \
    engine/scripting/coffeescriptconstructors.cpp \
    engine/scripting/qtscriptconstructors.cpp \
    inspector/coffeeexceptiondialog.cpp \
    engine/rendering/coffeerendercall.cpp \
    engine/scripting/coffeescriptengineagent.cpp \
    inspector/debugger/coffeecodeeditor.cpp \
    inspector/debugger/cutiesyntaxhighlighter.cpp \
    opengl/context/coffeerendererbase.cpp \
    opengl/context/coffeegpumonitor.cpp \
    general/eventloop/coffeefunctionqueuerunner.cpp \
    opengl/context/coffeeglfwcontextmanager.cpp \
    opengl/context/glfwnativefunctions.cpp \
    inspector/editor/coffeegameeditor.cpp \
    coffeegameenvironment.cpp \
    inspector/coffeeobjectexplorer.cpp \
    general/data/coffeepropertycontainer.cpp \
    inspector/debugger/coffeescriptexceptionhandler.cpp \
    inspector/debugger/coffeedebugview.cpp \
    inspector/information/coffeesystemmonitor.cpp \
    inspector/editor/coffeeeditorshell.cpp \
    general/scripting/coffeebasescriptenvironment.cpp \
    inspector/information/coffeemessagelogger.cpp \
    engine/scripting/coffeescriptfilehandler.cpp

HEADERS += \
    engine/models/coffeemesh.h \
    engine/objects/coffeeobjectfactory.h \
    engine/physics/physicsobject.h \
    engine/rendering/coffeeadvancedloop.h \
    general/data/coffeegameasset.h \
    general/data/numbercontainer.h \
    general/input/coffeejoystick.h \
    general/input/coffeeplayercontroller.h \
    general/common.h \
    general/filehandler.h \
    general/qstringfunctions.h \
    opengl/components/coffeecamera.h \
    opengl/components/coffeeframebufferobject.h \
    opengl/components/coffeematerial.h \
    opengl/components/coffeeomnilight.h \
    opengl/components/coffeetexture.h \
    opengl/components/coffeeworldopts.h \
    opengl/components/shadercontainer.h \
    opengl/context/coffeerenderer.h \
    opengl/context/qcoffeerenderer.h \
    opengl/context/qcoffeewindow.h \
    opengl/helpers/renderingmethods.h \
    opengl/helpers/texturehelper.h \
    opengl/rendering/renderloop.h \
    tests/boxtest.h \
    tests/CubeScape.h \
    tests/glutils.h \
    tests/RawFile.h \
    coffeelogger.h \
    engine/physics/genericphysicsinterface.h \
    engine/physics/bulletphysics.h \
    engine/objects/coffeeoutputsurface.h \
    engine/physics/physicsdescriptor.h \
    inspector/coffeeinspector.h \
    engine/objects/coffeeparticlesystem.h \
    inspector/coffeerendererinspector.h \
    inspector/graphplotter.h \
    engine/objects/coffeeobject.h \
    engine/objects/coffeestandardobject.h \
    engine/objects/coffeeskybox.h \
    engine/models/coffeeinstancecontainer.h \
    engine/data/coffeeassetimporter.h \
    inspector/coffeescriptterminal.h \
    engine/scripting/qscriptvectorvalue.h \
    engine/scripting/coffeescriptengine.h \
    engine/scripting/coffeeinputevent.h \
    opengl/components/framebuffers/coffeeframebufferbaseclass.h \
    engine/data/coffeeresource.h \
    engine/ai/coffeeneuralnet.h \
    engine/models/coffeeskeleton.h \
    engine/compute/coffeecomputeresource.h \
    engine/compute/coffeetransformcomputer.h \
    opengl/helpers/coffeeperlinnoisesource.h \
    engine/interface/coffeedrawableelement.h \
    engine/shaders/coffeeuniformsetter.h \
    opengl/components/coffeebuffer.h \
    engine/models/coffeevertexarrayobject.h \
    opengl/components/coffeeuniformblock.h \
    engine/rendering/coffeerendergraph.h \
    engine/data/vectors/matrix4value.h \
    engine/data/vectors/vector3value.h \
    engine/data/vectors/quatvalue.h \
    engine/data/vectors/scalarvalue.h \
    engine/data/vectors/vector2value.h \
    engine/data/vectors/vector4value.h \
    engine/scripting/coffeescriptconstructors.h \
    engine/scripting/qtscriptconstructors.h \
    inspector/coffeeexceptiondialog.h \
    engine/rendering/coffeerendercall.h \
    engine/scripting/coffeescriptengineagent.h \
    inspector/debugger/coffeecodeeditor.h \
    inspector/debugger/cutiesyntaxhighlighter.h \
    opengl/context/coffeerendererbase.h \
    opengl/context/coffeegpumonitor.h \
    general/eventloop/coffeefunctionqueuerunner.h \
    opengl/context/coffeeglfwcontextmanager.h \
    opengl/context/glfwnativefunctions.h \
    inspector/editor/coffeegameeditor.h \
    coffeegameenvironment.h \
    inspector/coffeeobjectexplorer.h \
    general/data/coffeepropertycontainer.h \
    inspector/debugger/coffeescriptexceptionhandler.h \
    inspector/debugger/coffeedebugview.h \
    inspector/information/coffeesystemmonitor.h \
    inspector/editor/coffeeeditorshell.h \
    general/scripting/coffeebasescriptenvironment.h \
    general/scripting/scripthelpers.h \
    inspector/information/coffeemessagelogger.h \
    engine/scripting/coffeescriptfilehandler.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../glbinding-library/release/ -lglbinding
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../glbinding-library/debug/ -lglbinding
else:unix: LIBS += -L$$PWD/../glbinding-library/ -lglbinding

INCLUDEPATH += $$PWD/libs/glbinding/source/glbinding/include
DEPENDPATH += $$PWD/libs/glbinding/source/glbinding/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../glfw-library/src/release/ -lGL -lGLEW -lglfw3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../glfw-library/src/debug/ -lGL -lGLEW -lglfw3
else:unix: LIBS += -L$$PWD/../glfw-library/src/ -lm -lrt -lGL  -lpthread -lglfw3 -lX11 -ldl -lXxf86vm -lXinerama -lXcursor -lXrandr -lXi

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
    inspector/coffeeinspector.ui \
    inspector/coffeerendererinspector.ui \
    inspector/coffeescriptterminal.ui \
    inspector/coffeeexceptiondialog.ui \
    inspector/editor/coffeegameeditor.ui \
    inspector/debugger/coffeedebugview.ui \
    inspector/information/coffeemessagelogger.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../assimp-library/code/release/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../assimp-library/code/debug/ -lassimp
else:unix: LIBS += -L$$PWD/../assimp-library/code/ -lassimp

INCLUDEPATH += $$PWD/libs/assimp/include
DEPENDPATH += $$PWD/libs/assimp/include

