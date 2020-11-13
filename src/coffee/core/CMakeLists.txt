find_package ( RapidJson REQUIRED )
find_package ( TinyXML2 REQUIRED )

add_library ( CoreCommon INTERFACE )

set ( CPPCODEC_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/cppcodec )

target_include_directories ( CoreCommon INTERFACE
    $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core_include>
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
# libz compression and whatever compression Windows has
#
if(NOT EMSCRIPTEN AND NOT APPLE)
    dependency_get (
        SOURCE hbirchtree/native-library-bundle
        TAG ${NATIVE_LIB_BUNDLE_TAG}
        NAMES
            zlib
        )
    find_package ( zlib )
endif()

dependency_get (
    SOURCE hbirchtree/native-library-bundle
    TAG ${NATIVE_LIB_BUNDLE_TAG}
    NAMES
        zstd
        lz4
    )
find_package ( lz4 CONFIG )
find_package ( zstd CONFIG )

coffee_library (
    TARGET CoreCompression

    SOURCES
    private/datastorage/compression/libz.cpp

    LIBRARIES CoreCommon

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/compression>
    HEADER_BASE coffee
    )

if(lz4_FOUND)
    target_sources ( CoreCompression PRIVATE
        private/datastorage/compression/lz4_codec.cpp
        )
    target_link_libraries ( CoreCompression PUBLIC
        lz4::lz4 lz4::lz4hc
        )
    target_compile_definitions ( CoreCompression PUBLIC
        -DCOFFEE_COMPRESS_LZ4
        )
    dependency_link (
        TARGET CoreCompression
        DEPENDENCIES lz4
        )
endif()
if(zstd_FOUND)

endif()

if(TARGET zlib::zlibstatic AND NOT EMSCRIPTEN AND NOT APPLE)
    target_link_libraries ( CoreCompression PUBLIC
        zlib::zlibstatic
        )
    dependency_link (
        TARGET CoreCompression
        DEPENDENCIES
            zlib
        )
elseif(ANDROID)
    target_link_libraries ( CoreCompression PUBLIC
        z
        )
elseif(APPLE OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
    target_link_libraries ( CoreCompression PUBLIC
        z
        )
else()
    target_sources ( CoreCompression PRIVATE
        zlib/inflate.c
        zlib/deflate.c

        zlib/infback.c
        zlib/inffast.c
        zlib/inftrees.c
        zlib/trees.c
        zlib/adler32.c
        zlib/crc32.c
        zlib/zutil.c
        )
    target_include_directories ( CoreCompression PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib/include>
        )
endif()

if(EMSCRIPTEN)
    target_compile_options ( CoreCompression PUBLIC
        "-s" USE_ZLIB=1
       )
   target_link_options ( CoreCompression PUBLIC
        "-s" USE_ZLIB=1
       )
endif()

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
    private/terminal/table_print.cpp

    LIBRARIES CoreCommon

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/terminal>
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

coffee_library (
    TARGET CoreObject

    SOURCES
    private/base/cobject.cpp

    LIBRARIES CoreCommon

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/object>
    HEADER_BASE coffee/core
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
    LIBRARIES CoreCommon CoreProfiling
    SOURCES private/components/allocators.cpp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/components>
    HEADER_BASE coffee
    )

target_precompile_headers ( EntComp PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:coffee/components/components.h>
    )

#
# Unit test librar that dies properly when a test fails
#
coffee_library (
    TARGET Testing
    SOURCES private/unit_tests/framework.cpp
    LIBRARIES CoreTerminal CoreCommon
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/coffee/core/unit_tests>
    HEADER_BASE coffee/core
    )

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

if(TARGET SDLPowerInfo)
    if(NOT TARGET SDL2)
        dependency_get (
            SOURCE hbirchtree/native-library-bundle
            TAG ${NATIVE_LIB_BUNDLE_TAG}
            NAMES sdl2=SDL2
            )

        find_package ( SDL2 CONFIG QUIET )
    endif()
#    target_link_libraries ( CoreCommon INTERFACE
#        SDLPowerInfo
#        )
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

header_install (
    Core
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface_include>
    ""
    )
