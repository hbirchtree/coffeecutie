#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T17:35:23
#
#-------------------------------------------------

QT -= core concurrent

TARGET = CoffeeCutie
CONFIG += console c++11
CONFIG -= app_bundle

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../assimp-library/code/release/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../assimp-library/code/debug/ -lassimp
else:unix: LIBS += -L$$PWD/../assimp-library/code/ -lassimp

INCLUDEPATH += $$PWD/libs/assimp/include
DEPENDPATH += $$PWD/libs/assimp/include

SOURCES += main.cpp \
    tests/CubeScape.cpp \
    tests/glutils.cpp \
    tests/RawFile.cpp \
    coffeelogger.cpp \
    coffee/display/cdrendererbase.cpp \
    coffee/cobject.cpp \
    coffee_impl/display/cglfwrenderer.cpp \
    coffee_impl/display/cglfwnativefuncs.cpp \
    coffee_impl/context/cdrenderer.cpp \
    coffee_impl/functional/cqueuerunner.cpp \
    coffee_impl/graphics/cshader.cpp \
    coffee_impl/assimp/cassimpimporters.cpp \
    coffee_impl/storage/cdatastore.cpp

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
    coffee_impl/display/cglfwrenderer.h \
    coffee/cinput.h \
    coffee_impl/display/cglfwrenderer_eventhandlers.h \
    coffee_impl/display/cglfwnativefuncs.h \
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
    coffee_impl/assimp/cassimptypes.h
