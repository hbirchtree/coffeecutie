if(NOT TARGET StandardLibs)
    # Some variables
    set ( CORE_INCLUDE_DIR )
    set ( CORE_EXTRA_LIBRARIES )

    # Platform-specific target options

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
        # Libunwind is used to print function names at runtime
        # Windows does not support this library
        # Android's Bionic does not grant access to unwind, even though it is there. Bad Google, bad!
        if(NOT DEFINED LIBUNWIND_INCLUDE_DIR)
            find_package(Unwind QUIET )
        endif()
        if (LIBUNWIND_FOUND)
            list ( APPEND CORE_INCLUDE_DIR
                $<BUILD_INTERFACE:${LIBUNWIND_INCLUDE_DIR}>
                )
            list ( APPEND CORE_EXTRA_LIBRARIES ${LIBUNWIND_LIBRARIES} )
        endif()
    endif()

    if(BUILD_SDL2)
        if(SDL_POWER_PLUGIN_ENABLED OR WIN_UWP)
            # We use SDL2 for some platform functionality, like power info
            # On Android, it is also used to read assets and
            #  acquiring device info.
            if(NOT DEFINED SDL2_INCLUDE_DIR)
                find_package(SDL2 REQUIRED)
            endif()
            if(SDL2_INCLUDE_DIR)
                list ( APPEND CORE_INCLUDE_DIR
                    $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
                    )
                if(NOT ${SDL2_LIBRARY} MATCHES ".framework")
                    list ( APPEND CORE_INCLUDE_DIR
                        $<INSTALL_INTERFACE:include/SDL2>
                        )
                endif()
            endif()
            #    list ( APPEND CORE_EXTRA_LIBRARIES ${SDL2_LIBRARY} )
        endif()
    endif()

    if(APPLE)
        list ( APPEND CORE_EXTRA_LIBRARIES
            "-framework AudioToolbox"
            "-framework CoreAudio"
            "-framework CoreFoundation"
            "-framework Foundation"
            )
    endif()

    if(ANDROID)
        # Add the Android logging library, as well as android and GLESv*
        list ( APPEND CORE_EXTRA_LIBRARIES
            # Logging and basic Android functions
            log android
            )
    endif()

    if(WIN32)
        # Don't know what this is, but it's necessary
        if(NOT WIN_UWP)
            list ( APPEND CORE_EXTRA_LIBRARIES
                # For some of the file API
                # This one is not available on UWP
                #pathcch

                # Core includes
                user32
                gdi32
                winmm
                imm32
                ole32
                oleaut32
                shell32
                version
                ws2_32
                wbemuuid
                )
        else()
            list ( APPEND CORE_EXTRA_LIBRARIES
                OneCore
                )
        endif()

        if(MINGW64)
            list ( APPEND CORE_EXTRA_LIBRARIES
                mingw32
                )
        endif()

        list ( APPEND CORE_EXTRA_LIBRARIES
            cabinet
            )
    endif()

    if(GAMECUBE)
        list ( APPEND CORE_EXTRA_LIBRARIES
            ogc
            )
    endif()

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND NOT ANDROID)
        # Used for thread details
        list ( APPEND CORE_EXTRA_LIBRARIES rt )
    endif()

    if(NOT WIN32 AND NOT MINGW AND NOT MSYS AND NOT EMSCRIPTEN)
        # Necessary for Linux and possibly OS X (latter is untested)
        if(NOT GAMECUBE)
            list ( APPEND CORE_EXTRA_LIBRARIES dl )
        endif()
        list ( APPEND CORE_EXTRA_LIBRARIES m)
    endif()

    if(NOT WIN32 AND NOT APPLE AND NOT EMSCRIPTEN AND NOT GAMECUBE AND NOT N64)
        find_package(Threads REQUIRED)
        list( APPEND CORE_EXTRA_LIBRARIES Threads::Threads )
    endif()

    add_library ( StandardLibs INTERFACE )

    target_link_libraries ( StandardLibs INTERFACE
        ${CORE_EXTRA_LIBRARIES}
        )
    target_include_directories ( StandardLibs INTERFACE
        ${CORE_INCLUDE_DIR}
        )

    if(EMSCRIPTEN)
        target_compile_options ( StandardLibs INTERFACE
            "-s            " ALLOW_MEMORY_GROWTH=1
            "-s           " SAFE_HEAP=1
            "-s          " DISABLE_EXCEPTION_CATCHING=0
            "-s         " DEMANGLE_SUPPORT=1
            "--no-heap-copy"
            "-fPIC"
           )
       target_link_options ( StandardLibs INTERFACE
            "-s            " ALLOW_MEMORY_GROWTH=1
            "-s           " SAFE_HEAP=1
            "-s          " DISABLE_EXCEPTION_CATCHING=0
            "-s         " DEMANGLE_SUPPORT=1
            "--no-heap-copy"
            "-fPIC"
           )
    endif()
endif()

message ( STATUS "Standardlibs in use: ${CORE_EXTRA_LIBRARIES}" )
message ( STATUS "Standard includes in use: ${CORE_INCLUDE_DIR}" )
