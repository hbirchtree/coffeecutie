find_package ( RapidJson REQUIRED )
find_package ( TinyXML2 REQUIRED )

coffee_library (
    TARGET XML
    SOURCES
    # We slap tinyXML2 in here, just cause
    ${COFFEE_EXT_LIBRARY_DIR}/tinyxml2/tinyxml2.cpp

    HEADER_DIRS
    $<INSTALL_INTERFACE:include/tinyxml2/>
    )

target_include_directories ( XML PUBLIC
    $<BUILD_INTERFACE:${TINYXML2_INCLUDE_DIR}>
    )

coffee_library (
    TARGET CoreBase

    SOURCES
    #
    # Core functionality, state and etc.
    #
    private/extern_storage.cpp

    # Thread code
    private/types/tdef/thread_glue.cpp

    #
    # Debug functions
    #
    private/base/cdebug.cpp
    private/base/debugprinter.cpp
    private/base/jsonlogger.cpp
    private/base/jsonprofile.cpp

    #
    # to_string(...) functions
    #
    private/strings.cpp

    #
    # Internal platform layer, cross-platform
    #

    # Argument handling
    private/argument_handling.cpp

    #
    # Task queues, for easier time scheduling
    #
    private/task_queue/task.cpp

    #
    # File-related functionality
    #
    private/base/cfiles.cpp

    LIBRARIES
    PeripheralLib
    PeripheralPlatform

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    $<INSTALL_INTERFACE:include>
    )

target_include_directories ( CoreBase PUBLIC
    $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    )

#
# libz compression and whatever compression Windows has
#
coffee_library (
    TARGET CoreCompression

    SOURCES
    private/datastorage/compression/libz.cpp

    LIBRARIES CoreBase
    )

#
# Virtual filesystem
#
coffee_library (
    TARGET CoreVirtFS

    SOURCES
    private/datastorage/binary/virtualfs.cpp

    LIBRARIES
    CoreBase
    CoreCompression
    )

#
# EventApplication C wrapper
#
coffee_library (
    TARGET CoreEventApplicationC

    SOURCES
    private/base/eventapplication_wrapper.cpp
    private/base/cdisplay.cpp

    LIBRARIES CoreBase
    )

#
# Utilities for printing in the terminal
#
coffee_library (
    TARGET CoreTerminal

    SOURCES
    private/terminal/table_print.cpp

    LIBRARIES CoreBase
    )

#
# Generic application data storage, for application state
#
coffee_library (
    TARGET CoreSaving

    SOURCES
    private/coffee_saving.cpp

    LIBRARIES CoreBase
    )

#
# Profiling export to Chrome trace format
#
coffee_library (
    TARGET CoreProfiling

    SOURCES
    private/profiler/profiling_export.cpp

    LIBRARIES CoreBase
    )

#
# Avoid this
#
coffee_library (
    TARGET CoreOldBlob

    SOURCES
    # Object ownership and labeling
    private/base/cobject.cpp

    LIBRARIES CoreBase
    )

#
# Custom INI reader/writer
#
coffee_library (
    TARGET INI
    LIBRARIES CoreBase
    SOURCES private/datastorage/text/ini/ciniparser.cpp
    )

#
# Unit test librar that dies properly when a test fails
#
coffee_library (
    TARGET Testing
    SOURCES private/unit_tests/framework.cpp
    LIBRARIES CoreBase
    )

if(LOWFAT_BUILD)
    target_compile_definitions (  CoreBase PUBLIC
        -DCOFFEE_LOWFAT
        )
endif()

add_library ( RapidJSON  INTERFACE )
add_library ( Core INTERFACE )

install(
    TARGETS RapidJSON Core
    EXPORT ${PROJECT_NAME}
    )

register_library ( Core
    "${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core" )

target_link_libraries ( Core INTERFACE
    CoreBase
    CoreCompression
    CoreEventApplicationC
    CoreOldBlob
    CoreProfiling
    CoreSaving
    CoreTerminal
    CoreVirtFS
    PeripheralThreadNaming
    )

if(TARGET SDLPowerInfo)
    target_link_libraries ( Core INTERFACE
        SDLPowerInfo
        )
endif()

#
#
# Alternative thread implementations
# Used for platforms without std::thread/std::mutex support
# Implemented here:
#  - Gamecube/Wii LWP
#
#
if(GAMECUBE)
    target_link_libraries ( CoreBase PUBLIC
        GeckoThreads
        )
endif()

coffee_bundle (
    DESTINATION libs

    HEADER_DIRECTORIES
    ${RAPIDJSON_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
    )

coffee_bundle (
    HEADER_DIRECTORIES
    ${STANDARDLIBS_INCLUDE_DIR}
    )

coffee_bundle (
    FORCE
    HEADER_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/interface_include/coffee
    )
