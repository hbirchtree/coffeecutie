if(COFFEE_BUILD_QT)
    add_subdirectory ( qt-profiler )
endif()

add_subdirectory(endian-fixer)

if(COFFEE_BUILD_BOT)
    add_subdirectory(build-bot)
endif()
