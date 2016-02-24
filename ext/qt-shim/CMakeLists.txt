include ( QtIncludes )

coffee_add_library ( QtShimCore
    private/qtinit.cpp )

target_link_libraries ( QtShimCore Qt5::Widgets)

add_subdirectory ( qt-node-editor )
add_subdirectory ( qt-dialogs )
