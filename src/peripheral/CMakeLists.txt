add_library ( PeripheralLib INTERFACE )

target_include_directories ( PeripheralLib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

install (
    TARGETS PeripheralLib
    EXPORT ${PROJECT_NAME}
    )

register_library ( PeripheralLib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

############################################################
# STL extensions, replacing STL where it does not exist
############################################################

if(GAMECUBE)
    coffee_library (
        TARGET GeckoThreads
        LIBRARIES
        SOURCES
        stl_replacements/gecko/threads.cpp
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
