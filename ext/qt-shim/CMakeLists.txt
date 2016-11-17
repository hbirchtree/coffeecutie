include ( QtIncludes )

coffee_add_library ( CoffeeQtShimCore private/qtinit.cpp "Qt5::Widgets" )

add_subdirectory ( qt-node-editor )
add_subdirectory ( qt-dialogs )
