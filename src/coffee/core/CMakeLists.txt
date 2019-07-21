find_package ( RapidJson REQUIRED )
find_package ( TinyXML2 REQUIRED )

add_library ( CoreCommon INTERFACE )

target_include_directories ( CoreCommon INTERFACE
    $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_link_libraries ( CoreCommon INTERFACE
    PeripheralLib
    PeripheralPlatform
    )

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
    TARGET CoreDebug

    SOURCES
    #
    # Debug functions
    #
    private/base/cdebug.cpp
    private/base/debugprinter.cpp

    #
    # to_string(...) functions
    #
    private/strings.cpp

    LIBRARIES CoreCommon

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

    $<INSTALL_INTERFACE:include>
    )

coffee_library (
    TARGET CoreState

    SOURCES
    #
    # Core functionality, state and etc.
    #
    private/extern_storage.cpp

    LIBRARIES
    CoreCommon
    CoreDebug
    )

coffee_library (
    TARGET CoreFile

    SOURCES
    #
    # File-related functionality
    #
    private/base/cfiles.cpp

    LIBRARIES
    CoreCommon
    )

coffee_library (
    TARGET CoreQueue

    SOURCES
    #
    # Task queues, for easier time scheduling
    #
    private/task_queue/task.cpp

    LIBRARIES
    CoreCommon
    PeripheralThreadNaming
    )

coffee_library (
    TARGET CoreJsonLogger

    SOURCES
    private/base/jsonlogger.cpp
    private/base/jsonprofile.cpp

    LIBRARIES
    CoreCommon PeripheralThreadNaming
    )

#
# libz compression and whatever compression Windows has
#
coffee_library (
    TARGET CoreCompression

    SOURCES
    private/datastorage/compression/libz.cpp

    LIBRARIES CoreCommon
    )

#
# Virtual filesystem
#
coffee_library (
    TARGET CoreVirtFS

    SOURCES
    private/datastorage/binary/virtualfs.cpp

    LIBRARIES
    CoreCommon
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

    LIBRARIES CoreCommon
    )

#
# Utilities for printing in the terminal
#
coffee_library (
    TARGET CoreTerminal

    SOURCES
    private/terminal/table_print.cpp

    LIBRARIES CoreCommon
    )

#
# Generic application data storage, for application state
#
coffee_library (
    TARGET CoreSaving

    SOURCES
    private/coffee_saving.cpp

    LIBRARIES CoreCommon
    )

#
# Profiling export to Chrome trace format
#
coffee_library (
    TARGET CoreProfiling

    SOURCES
    private/profiler/profiling_export.cpp

    LIBRARIES CoreCommon
    )

#
# Avoid this
#
coffee_library (
    TARGET CoreOldBlob

    SOURCES
    # Object ownership and labeling
    private/base/cobject.cpp

    LIBRARIES CoreCommon
    )

#
# Custom INI reader/writer
#
coffee_library (
    TARGET INI
    LIBRARIES CoreCommon
    SOURCES private/datastorage/text/ini/ciniparser.cpp
    )

#
# Entity-Component-System implementation
#
coffee_library (
    TARGET EntComp
    LIBRARIES CoreCommon
    SOURCES private/components/allocators.cpp
    )

#
# Unit test librar that dies properly when a test fails
#
coffee_library (
    TARGET Testing
    SOURCES private/unit_tests/framework.cpp
    LIBRARIES CoreCommon
    )

if(LOWFAT_BUILD)
    target_compile_definitions (  CoreCommon INTERFACE
        -DCOFFEE_LOWFAT
        )
endif()

add_library ( RapidJSON  INTERFACE )
add_library ( Core INTERFACE )

add_export ( Core       "" )
add_export ( RapidJSON  "" )
add_export ( CoreCommon "" )

target_link_libraries ( Core INTERFACE
    CoreCommon
    CoreCompression
    CoreEventApplicationC
    CoreOldBlob
    CoreProfiling
    CoreSaving
    CoreTerminal
    CoreVirtFS
    CoreQueue
    CoreFile
    CoreJsonLogger
    CoreState

    PeripheralThreadNaming
    )

if(TARGET SDLPowerInfo)
    target_link_libraries ( CoreCommon INTERFACE
        SDLPowerInfo
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
