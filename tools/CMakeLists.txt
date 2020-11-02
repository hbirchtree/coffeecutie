if(NOT ANDROID AND NOT IOS AND NOT WIN32 AND NOT EMSCRIPTEN
        AND NOT LOWFAT_BUILD AND NOT GAMECUBE)
    add_subdirectory ( data-management/pressure-cooker )
    add_subdirectory ( crash-recovery )
endif()

add_subdirectory ( system-info )

if(TARGET ASIO)
    add_subdirectory ( natty-puncher )
endif()
