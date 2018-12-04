find_package ( StandardLibs REQUIRED )
find_package ( RapidJson REQUIRED )
find_package ( TinyXML2 REQUIRED )

coffee_library (
    TARGET CoffeeXML
    SOURCES
    # We slap tinyXML2 in here, just cause
    ${COFFEE_EXT_LIBRARY_DIR}/tinyxml2/tinyxml2.cpp

    HEADER_DIRS
    $<BUILD_INTERFACE:${TINYXML2_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/tinyxml2/>
    )

coffee_library (
    TARGET CoffeeCore_Base

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
    ${STANDARDLIBS_LIBRARIES}
    PeripheralLib
    PeripheralPlatform

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>

    $<INSTALL_INTERFACE:include>
    )

#
# libz compression and whatever compression Windows has
#
coffee_library (
    TARGET CoffeeCore_Compression

    SOURCES
    private/datastorage/compression/libz.cpp

    LIBRARIES CoffeeCore_Base
    )

#
# Virtual filesystem
#
coffee_library (
    TARGET CoffeeCore_VirtFS

    SOURCES
    private/datastorage/binary/virtualfs.cpp

    LIBRARIES
    CoffeeCore_Base
    CoffeeCore_Compression
    )

#
# EventApplication C wrapper
#
coffee_library (
    TARGET CoffeeCore_EventApplicationC

    SOURCES
    private/base/eventapplication_wrapper.cpp
    private/base/cdisplay.cpp

    LIBRARIES CoffeeCore_Base
    )

#
# Useful system information, such as display DPI and system name
#
#coffee_library (
#    TARGET CoffeeCore_PlatformData

#    SOURCES
#    private/platform_data.cpp

#    LIBRARIES CoffeeCore_Base
#    )

#
# Utilities for printing in the terminal
#
coffee_library (
    TARGET CoffeeCore_TerminalPrinting

    SOURCES
    private/terminal/table_print.cpp

    LIBRARIES CoffeeCore_Base
    )

#
# Generic application data storage, for application state
#
coffee_library (
    TARGET CoffeeCore_Saving

    SOURCES
    private/coffee_saving.cpp

    LIBRARIES CoffeeCore_Base
    )

#
# Profiling export to Chrome trace format
#
coffee_library (
    TARGET CoffeeCore_Profiling

    SOURCES
    private/profiler/profiling_export.cpp

    LIBRARIES CoffeeCore_Base
    )

#
# Avoid this
#
coffee_library (
    TARGET CoffeeCore_OldBlob

    SOURCES
    # Object ownership and labeling
    private/base/cobject.cpp

    LIBRARIES CoffeeCore_Base
    )

#
# Custom INI reader/writer
#
coffee_library (
    TARGET CoffeeINI
    LIBRARIES CoffeeCore_Base
    SOURCES private/datastorage/text/ini/ciniparser.cpp
    )

#
# Unit test librar that dies properly when a test fails
#
coffee_library (
    TARGET CoffeeTesting
    SOURCES private/unit_tests/framework.cpp
    LIBRARIES CoffeeCore_Base
    )

if(LOWFAT_BUILD)
    target_compile_definitions (  CoffeeCore_Base PUBLIC
        -DCOFFEE_LOWFAT
        )
endif()

add_library ( RapidJSON  INTERFACE )
add_library ( CoffeeCore INTERFACE )

install(
    TARGETS RapidJSON CoffeeCore
    EXPORT ${PROJECT_NAME}
    )

register_library ( CoffeeCore
    "${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core" )

target_include_directories ( CoffeeCore_Base PUBLIC

    $<BUILD_INTERFACE:${STANDARDLIBS_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${STANDARDLIBS_INCLUDE_DIR}>
    )

target_link_libraries ( CoffeeCore INTERFACE
    CoffeeCore_Base
    CoffeeCore_Compression
    CoffeeCore_EventApplicationC
    CoffeeCore_OldBlob
#    CoffeeCore_PlatformData
    CoffeeCore_Profiling
    CoffeeCore_Saving
    CoffeeCore_TerminalPrinting
    CoffeeCore_VirtFS
    PeripheralThreadNaming
    )

if(TARGET CoffeeSDLPowerInfo)
    target_link_libraries ( CoffeeCore INTERFACE
        CoffeeSDLPowerInfo
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
    target_link_libraries ( CoffeeCore_Base PUBLIC
        GeckoThreads
        )
endif()

coffee_bundle_includes (
    ${STANDARDLIBS_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
    )
