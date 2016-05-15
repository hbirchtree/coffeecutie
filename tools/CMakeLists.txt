if(COFFEE_BUILD_QT)
    add_subdirectory ( performance-tools/qt-profiler )
    add_subdirectory ( build-daemon/build-viewer-qml )
endif()

add_subdirectory(data-management/endian-fixer)

if(COFFEE_BUILD_BOT)
    add_subdirectory(build-daemon/build-bot)
endif()
