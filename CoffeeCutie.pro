#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T17:35:23
#
#-------------------------------------------------

QT -= core concurrent

TARGET = CoffeeCutie
CONFIG += console c++11
CONFIG -= app_bundle

CONFIG += sdl2_context

LIBS += -lunwind

csharp {
    #Use CONFIG += csharp to build as library, requires rebuild for -fPIC flag
    TEMPLATE = lib
    TARGET = CoffeeSharp
    message(Configured for CoffeeSharp)
    QMAKE_CXXFLAGS += -fPIC
    #Remember: ar rcs libCoffeeSharp.a *.o
}else{
    TEMPLATE = app
}

INCLUDEPATH += $$PWD/include

#GLBINDING LIBRARY
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../glbinding-library/release/ -lglbinding
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../glbinding-library/debug/ -lglbinding
else:unix: LIBS += -L$$PWD/../glbinding-library/ -lglbinding

INCLUDEPATH += $$PWD/libs/glbinding/source/glbinding/include
DEPENDPATH += $$PWD/libs/glbinding/source/glbinding/include

glfw_context {
    #GLFW LIBRARY
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

    HEADERS += \
        coffee_impl/display/cglfwrenderer.h \
        coffee_impl/display/coffeeglfw/cglfwrenderer_eventhandlers.h \
        coffee_impl/display/coffeeglfw/cglfwnativefuncs.h
    SOURCES += \
        coffee_impl/display/cglfwrenderer.cpp \
        coffee_impl/display/coffeeglfw/cglfwnativefuncs.cpp
}
#END OF GLFW

#SDL2 LIBRARY
sdl2_context {
    unix: LIBS += -lSDL2

    SOURCES += \
        coffee_impl/display/csdl2renderer.cpp
    HEADERS += \
        coffee_impl/display/csdl2renderer.h \
        coffee_impl/display/coffeesdl2/sdl2datatypes.h
}
#END OF SDL2

#BULLET LIBRARY
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bullet-library/src/release/ -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bullet-library/src/debug/ -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
else:unix: LIBS += -L$$PWD/../bullet-library/src/ -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath

INCLUDEPATH += $$PWD/libs/bullet/src
DEPENDPATH += $$PWD/libs/bullet/src

#ASSIMP LIBRARY
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../assimp-library/code/release/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../assimp-library/code/debug/ -lassimp
else:unix: LIBS += -L$$PWD/../assimp-library/code/ -lassimp

INCLUDEPATH += $$PWD/libs/assimp/include
DEPENDPATH += $$PWD/libs/assimp/include

#YES, I DO BELIEVE CAPS = YELLING

SOURCES += main.cpp \
    tests/CubeScape.cpp \
    tests/glutils.cpp \
    tests/RawFile.cpp \
    coffeelogger.cpp \
    coffee/display/cdrendererbase.cpp \
    coffee/cobject.cpp \
    coffee_impl/context/cdrenderer.cpp \
    coffee_impl/functional/cqueuerunner.cpp \
    coffee_impl/graphics/cshader.cpp \
    coffee_impl/assimp/cassimpimporters.cpp \
    coffee_impl/storage/cdatastore.cpp \
    coffee_impl/graphics/cgraphicsdata.cpp \
    coffee_impl/context/cglbindingrenderer.cpp \
    coffee_impl/rendering/cmultidrawgraph.cpp \
    coffee_impl/graphics/cuniformchunk.cpp

HEADERS += \
    tests/CubeScape.h \
    tests/glutils.h \
    tests/RawFile.h \
    coffeelogger.h \
    coffee.h \
    coffee/display/cdrendererbase.h \
    coffee/cobject.h \
    coffee/cdebug.h \
    coffee/cfunctional.h \
    coffee/cdisplay.h \
    coffee/cinput.h \
    coffee_impl/context/cdrenderer.h \
    coffee_impl/functional/cqueuerunner.h \
    coffee_impl/graphics/cgraphicswrappers.h \
    coffee_impl/graphics/cshader.h \
    coffeesharp.h \
    coffee/cfiles.h \
    coffee/cregex.h \
    coffee_impl/assimp/cassimpimporters.h \
    coffee_impl/storage/cdatastore.h \
    coffee_impl/assimp/assimpfun.h \
    coffee_impl/assimp/cassimptypes.h \
    coffee_impl/assimp/cassimploader.h \
    coffee_impl/graphics/cgraphicsdata.h \
    unit_tests/data_types.h \
    coffee_impl/graphics/cbuffer.h \
    coffee_impl/graphics/glbinding.h \
    coffee_impl/graphics/cvertexarrayobject.h \
    coffee_impl/graphics/cuniformtypes.h \
    coffee_impl/graphics/cframebuffer.h \
    coffee_impl/graphics/ctexture.h \
    coffee_impl/context/cglbindingrenderer.h \
    plat/linux_wm.h \
    coffee_impl/graphics/ctransformcomputer.h \
    coffee_impl/rendering/cmultidrawgraph.h \
    coffee_impl/graphics/cuniformchunk.h \
    coffee_impl/display/coffeesdl2/sdl2helpers.h
