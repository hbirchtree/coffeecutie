include ( QtIncludes )

include_directories(
    ${CMAKE_SOURCE_DIR}/ext/qt-shim/qt-node-editor/include
    qt5
    )

set ( NodeEditorTest_SRCS
    main.cpp
    mainwindowtester.cpp
    )

list ( APPEND LIBRARIES
    CoffeeCore
    CoffeeQtNodeEditor
 )

coffee_add_example ( NodeEditorTest "Node Editor Test" "${NodeEditorTest_SRCS}" "${LIBRARIES}" )

target_link_libraries ( NodeEditorTest
    )
