if(COFFEE_BUILD_QT)
    add_subdirectory ( performance-tools/qt-profiler )
    add_subdirectory ( data-management/blam-data-viewer )
endif()

if(NOT ANDROID AND NOT IOS)
    add_subdirectory ( data-management/pressure-cooker )
endif()
