include ( QtIncludes )

include_directories(
    ${CMAKE_SOURCE_DIR}/ext/qt-shim/qt-node-editor/include
    qt5
    )

set ( NodeEditorTest_SRCS
    main.cpp
    mainwindowtester.cpp
    )

coffee_add_example ( NodeEditorTest "${NodeEditorTest_SRCS}" )

target_link_libraries ( NodeEditorTest
    CoffeeCore
    CoffeeQtNodeEditor
    )