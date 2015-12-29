set ( NodeEditorTest_SRCS
        main.cpp
        mainwindowtester.cpp
    )

find_package(Qt5Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
        ${CMAKE_SOURCE_DIR}/ext/qt-shim/qt-node-editor/include
        qt5
    )

add_executable ( NodeEditorTest ${NodeEditorTest_SRCS} )

target_link_libraries ( NodeEditorTest
                CoffeeCore
                CoffeeQtNodeEditor
        )

target_compile_features(NodeEditorTest PRIVATE cxx_variadic_templates)
