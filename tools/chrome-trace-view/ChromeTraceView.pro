TEMPLATE = app

QT += quick quickcontrols2

QMAKE_CXXFLAGS += -std=c++14 -s "BINARYEN_TRAP_MODE='clamp'"

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/main.cpp src/tracemodel.cpp
HEADERS += src/tracemodel.h

RESOURCES += qml/qml.qrc

QML_IMPORT_PATH =

QML_DESIGNER_IMPORT_PATH =

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
