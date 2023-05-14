find_package ( RapidJSON CONFIG REQUIRED )
find_package ( tinyxml2 CONFIG REQUIRED )

add_library ( CoreCommon INTERFACE )

set ( CPPCODEC_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/cppcodec )

target_include_directories ( CoreCommon INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core_include>
    )

target_link_libraries ( CoreCommon INTERFACE
#    rapidjson
    tinyxml2::tinyxml2
    )

header_install (
    CoreCommon
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core_include>
    ""
    )

target_link_libraries ( CoreCommon INTERFACE
    PeripheralLib
    PeripheralPlatform
    )

coffee_library (
    TARGET CoreDebug

    SOURCES
#        private/strings.cpp

    LIBRARIES CoreCommon PeripheralThreadNaming

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/debug>
    $<INSTALL_INTERFACE:include>
    HEADER_BASE coffee/core
    )

header_install (
    "CoreDebug"
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/strings>
    coffee
    )

coffee_library (
    TARGET CoreState

    SOURCES
    #
    # Core functionality, state and etc.
    #
    private/extern_storage.cpp

    LIBRARIES CoreCommon CoreJsonLogger CoreDebug PeripheralThreadNaming
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

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/files>
    HEADER_BASE coffee/core
    )

target_precompile_headers ( CoreFile PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/core/CFiles>
    $<$<COMPILE_LANGUAGE:CXX>:platforms/file.h>
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

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/task_queue>
    HEADER_BASE coffee/core
    )

target_precompile_headers ( CoreQueue PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/core/task_queue/task.h>
    )

coffee_library (
    TARGET CoreJsonLogger

    SOURCES
    private/base/jsonlogger.cpp
    private/base/jsonprofile.cpp

    LIBRARIES
    CoreDebug CoreCommon PeripheralThreadNaming CoreProfiling

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/logging>
    HEADER_BASE coffee/core
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
    CoreZ
    CoreZstd

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/virtfs>
    HEADER_BASE coffee
    )

#
# Utilities for printing in the terminal
#
coffee_library (
    TARGET CoreTerminal

    SOURCES
#    private/terminal/table_print.cpp

    LIBRARIES CoreCommon

    HEADER_DIRS
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/terminal>
    HEADER_BASE coffee/core
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

    LIBRARIES CoreDebug CoreCommon CoreFile

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/profiler>
    HEADER_BASE coffee/core
    )

#
# Entity-Component-System implementation
#
coffee_library (
    TARGET EntComp
    LIBRARIES CoreCommon CoreProfiling
    SOURCES private/components/allocators.cpp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/components>
    HEADER_BASE coffee
    )

target_precompile_headers ( EntComp PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/components/components.h>
    )

if(BUILD_TESTS)
    #
    # Unit test library that dies properly when a test fails
    #
    coffee_library (
        TARGET Testing
        SOURCES private/unit_tests/framework.cpp
        LIBRARIES CoreTerminal CoreCommon
        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/unit_tests>
        HEADER_BASE coffee/core
        )
endif()

if(LOWFAT_BUILD)
    target_compile_definitions (  CoreCommon INTERFACE
        -DCOFFEE_LOWFAT
        )
endif()

add_library ( CppCodec INTERFACE )
add_library ( RapidJSON  INTERFACE )
add_library ( Core INTERFACE )

add_export ( Core       "" )
add_export ( RapidJSON  "" )
add_export ( CoreCommon "" )

target_link_libraries ( Core INTERFACE
    CoreCommon
    CoreProfiling
    CoreSaving
    CoreTerminal
    CoreQueue
    CoreFile
    CoreState
    CoreJsonLogger

    PeripheralThreadNaming
    )

coffee_bundle (
    HEADER_DIRECTORIES
    ${STANDARDLIBS_INCLUDE_DIR}
    )

header_install (
    Core
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    ""
    )
