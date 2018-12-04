coffee_library (
    TARGET PeripheralLib
    SOURCES
    private/error_types.cpp
    private/build_info.cpp
    private/libc_functions.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( PeripheralLib PUBLIC
    )

############################################################
# STL extensions, replacing STL where it does not exist
############################################################

if(GAMECUBE)
    coffee_library (
        TARGET GeckoThreads
        LIBRARIES
        SOURCES
        stl_replacements/gekko/threads.cpp
        )
endif()

############################################################
# Backup thread naming
#  - for platforms where thread naming doesn't really exist
############################################################

coffee_library (
    TARGET PeripheralThreadNaming
    LIBRARIES CoffeeCore_Base
    SOURCES
    stl_extensions/thread_naming.cpp
    )

############################################################
# Platform layer
############################################################

add_subdirectory ( platform )
