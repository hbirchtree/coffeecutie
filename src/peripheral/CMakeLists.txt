find_package ( StandardLibs REQUIRED )

add_export ( StandardLibs "" )

coffee_library (
    TARGET PeripheralLib
    SOURCES
    private/error_types.cpp
    private/build_info.cpp
    private/libc_functions.cpp

    LIBRARIES
    StandardLibs

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_compile_definitions ( PeripheralLib PRIVATE
    -DCOFFEE_BUILD_STRING=${COFFEE_BUILD_STRING}
    )

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
        LIBRARIES
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
