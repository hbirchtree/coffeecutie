if(NOT ANDROID AND NOT IOS AND NOT WIN32 AND NOT EMSCRIPTEN
        AND NOT LOWFAT_BUILD)
    add_subdirectory ( data-management/pressure-cooker )
    add_subdirectory ( crash-recovery )
endif()
