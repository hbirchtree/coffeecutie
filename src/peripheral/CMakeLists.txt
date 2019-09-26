find_package ( StandardLibs REQUIRED )

add_export ( StandardLibs "" )

coffee_library (
    TARGET PeripheralLib
    SOURCES
    private/error_types.cpp
    private/build_info.cpp
    private/libc_functions.cpp
    private/pimpl_state.cpp

    LIBRARIES
    StandardLibs

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_compile_definitions ( PeripheralLib PRIVATE
    -DCOFFEE_BUILD_STRING=${COFFEE_BUILD_STRING}
    )

if(EMSCRIPTEN)
    if("${EMSCRIPTEN_SOURCEMAP_BASE}" STREQUAL "")
        set ( EMSCRIPTEN_SOURCEMAP_BASE "http://localhost:8000" )
    endif()

    set ( EMSCRIPTEN_SOURCEMAP_BASE "${EMSCRIPTEN_SOURCEMAP_BASE}"
        CACHE STRING "Emscripten sourcemap base URL"
        )

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options ( PeripheralLib PUBLIC
            -g4
            "-s                              " DEMANGLE_SUPPORT=1
            )
        target_link_options ( PeripheralLib PUBLIC
            -g4
            "-s                            " DEMANGLE_SUPPORT=1
            --source-map-base "${EMSCRIPTEN_SOURCEMAP_BASE}"
            )
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        target_compile_options ( PeripheralLib PUBLIC
            -g4 -O1
            "-s                                 " DEMANGLE_SUPPORT=1
            )
        target_link_options ( PeripheralLib PUBLIC
            -g4 -O1
            "-s                                " DEMANGLE_SUPPORT=1
            --source-map-base "${EMSCRIPTEN_SOURCEMAP_BASE}"
            )
    else()
        target_compile_options ( PeripheralLib PUBLIC
            -O3
            )
        target_link_options ( PeripheralLib PUBLIC
            -O3
            )
    endif()
endif()

############################################################
# STL extensions, replacing STL where it does not exist
############################################################


############################################################
# Backup thread naming
#  - for platforms where thread naming doesn't really exist
############################################################

coffee_library (
    TARGET PeripheralThreadNaming
    LIBRARIES PeripheralPlatform
    SOURCES
    stl_extensions/thread_naming.cpp
    )

#
#
# Alternative thread implementations
# Used for platforms without std::thread/std::mutex support
# Implemented here:
#  - Gamecube/Wii LWP
#
#
if(GAMECUBE)
    coffee_library (
        TARGET GeckoThreads
        LIBRARIES PeripheralLib
        SOURCES
        stl_replacements/gekko/threads.cpp
        )
    target_link_libraries ( PeripheralThreadNaming INTERFACE
        GeckoThreads
        )
endif()

############################################################
# Platform layer
############################################################

add_subdirectory ( platform )
