if(NOT ANDROID AND NOT IOS AND NOT WIN32 AND NOT EMSCRIPTEN)
    add_subdirectory ( data-management/pressure-cooker )
endif()
