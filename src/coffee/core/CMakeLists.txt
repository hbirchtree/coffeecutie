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
    # Operating system signal handling
    private/coffee_signals.cpp
    # Library versioning
    private/application_data.cpp

    # Thread code
    private/types/tdef/thread_glue.cpp

    #
    # Debug functions
    #
    private/base/cdebug.cpp
    private/base/debugprinter.cpp
    private/base/jsonlogger.cpp
    private/base/jsonprofile.cpp

    # Wrapper around std::regex
    private/base/cregex.cpp

    #
    # to_string(...) functions
    #
    private/base/strings/extensionresolvers.cpp
    private/base/strings/information_extensions.cpp
    private/base/strings/vector_print_extensions.cpp

    #
    # Internal platform layer, cross-platform
    #

    # Argument handling
    private/argument_handling.cpp
    private/types/cdef/infotypes.cpp
    private/plat/environment/argument_parse.cpp

    private/plat/environment/shared_func.cpp
    private/plat/memory/cmd_interface.cpp

    #
    # Task queues, for easier time scheduling
    #
    private/task_queue/task.cpp

    #
    # File-related functionality
    #
    private/plat/file/file_common.cpp
    private/base/cfiles.cpp
    private/base/url.cpp

    #
    # This one is pretty important, let's keep it here
    #
    private/plat/timing/profiling.cpp

    LIBRARIES
    ${STANDARDLIBS_LIBRARIES}

    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
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
# Linked only by applications, contains main()
#
coffee_library (
    TARGET CoffeeCore_Application

    SOURCES
    private/coffee.cpp
    private/plat/environment/application_start.cpp

    LIBRARIES CoffeeCore_Base
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
coffee_library (
    TARGET CoffeeCore_PlatformData

    SOURCES
    private/platform_data.cpp

    LIBRARIES CoffeeCore_Base
    )

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

    LIBRARIES CoffeeCore_Base CoffeeCore_PlatformData
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

target_compile_definitions ( CoffeeCore_Base PUBLIC
    -DCOFFEE_BUILD_STRING=${COFFEE_BUILD_STRING}
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

register_library ( CoffeeCore "${CMAKE_SOURCE_DIR}/include/coffee/core" )

target_include_directories ( CoffeeCore_Base PUBLIC
    $<BUILD_INTERFACE:${STANDARDLIBS_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${STANDARDLIBS_INCLUDE_DIR}>
    )

target_link_libraries ( CoffeeCore INTERFACE
    CoffeeCore_Base
    CoffeeCore_Compression
    CoffeeCore_EventApplicationC
    CoffeeCore_OldBlob
    CoffeeCore_PlatformData
    CoffeeCore_Profiling
    CoffeeCore_Saving
    CoffeeCore_TerminalPrinting
    CoffeeCore_VirtFS
    )

#
#
# SDL power management info
#
#
if(BUILD_SDL2 AND NOT POWERPC AND NOT IOS AND
        (ANDROID OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR WIN32))

    if(NOT TARGET SDL2)
        find_package ( SDL2 QUIET )
    endif()

    target_sources ( CoffeeCore_Base PRIVATE
        private/plat/environment/sdlpowerinfo.cpp
        )

    target_link_libraries ( CoffeeCore_Base PRIVATE
        SDL2
        )
endif()

#
#
# Android, there's a lot of weird stuff here because NativeActivity sucks
# Generally should not be touched
#
#
if(ANDROID)
    find_package( AndroidGlue REQUIRED )

    target_sources ( CoffeeCore_Application PRIVATE
        platform/and_main.cpp
        ${ANDROID_GLUE_SOURCES}
        )

    #
    # native_app_glue has a lot of spam in debug mode, we disable that
    #
    set_source_files_properties (
        ${ANDROID_GLUE_SOURCES}
        PROPERTIES
        COMPILE_DEFINITIONS "NDEBUG"
        )

    target_sources ( CoffeeCore_Base PRIVATE
        private/plat/environment/android/environment.cpp
        private/plat/sensor/android/sensor.cpp
        private/plat/file/linux/file.cpp

        ${NDK_HELPER_SOURCES}
        ${CPUFEATURES_SOURCES}

        private/plat/file/android/file.cpp

        # For reading process information
        private/plat/environment/linux/process.cpp
        private/plat/environment/linux/sysinfo.cpp
        private/plat/environment/android/sysinfo.cpp
        )

    target_compile_definitions ( CoffeeCore_Base PUBLIC
        -DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
        )

    target_compile_options ( CoffeeCore_Base PUBLIC
        # This flag gives a lot of false positives for aligned accesses
        -Wno-address-of-packed-member
        )

    target_include_directories ( CoffeeCore_Base PUBLIC
        $<BUILD_INTERFACE:${ANDROID_GLUE_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NDK_HELPER_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CPUFEATURES_INCLUDE_DIR}>
        )
endif()

#
#
# Apple iOS/OS X-specific code
#
#
if(APPLE)
    target_sources ( CoffeeCore_Base PRIVATE
        #
        # System information queries
        #
        private/plat/environment/osx/sysinfo.cpp
        private/plat/environment/osx/environment.cpp
        # Some functions are exposed by Obj-C
        private/plat/environment/osx/system_interface.m

        private/plat/file/osx/file.cpp
        )
endif()

#
#
# Portable POSIX code
#
#
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR ANDROID)
    target_sources ( CoffeeCore_Base PRIVATE

        #
        # POSIX environment functions
        #
        private/plat/environment/unix/environment.cpp

        #
        # Stacktrace using libunwind
        #
        private/plat/environment/unix/stacktrace.cpp

        #
        # Dynamic linking (libdl)
        #
        private/plat/linking/unix/loader.cpp

        #
        # File management
        #
        private/plat/file/unix/posix_shared.cpp
        private/plat/file/unix/file.cpp
        )

    find_package( Unwind QUIET )
    if(UNWIND_FOUND)
        add_definitions(
            -DLIBUNWIND_ENABLED
            )
    endif()
endif()

#
#
# Linux desktop-specific code
#
#
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    target_sources ( CoffeeCore_Base PRIVATE
        #
        # Linux-specific system calls and behavior all around
        #
        private/plat/environment/linux/environment.cpp
        private/plat/environment/linux/stacktrace.cpp
        private/plat/environment/linux/process.cpp
        private/plat/environment/linux/sysinfo.cpp

        private/plat/file/linux/file.cpp
        )
endif()

#
#
# Emscripten-specific code
#
#
if(EMSCRIPTEN)
    target_sources ( CoffeeCore_Base PRIVATE
        #
        # Like POSIX, but limited
        #
        private/plat/environment/unix/environment.cpp
        private/plat/environment/emscripten/sysinfo.cpp
        )
endif()

#
#
# Raspberry Pi-specific code
#
#
if(RASPBERRY)
    target_sources ( CoffeeCore_Base PRIVATE
        #
        # Extracts Raspberry Pi board name
        #
        private/plat/environment/raspberry/sysinfo.cpp
        )
endif()

#
#
# Maemo-specific code
#
#
if(MAEMO)
    target_sources ( CoffeeCore_Base PRIVATE
        #
        # Accelerometers, light sensors and etc.
        #
        private/plat/sensor/maemo/sensors.cpp
        )
endif()

#
#
# Windows Win32/UWP-specific code
#
#
if(WIN32)
    target_sources ( CoffeeCore_Base PRIVATE
        private/plat/environment/windows/environment.cpp
        private/plat/environment/windows/sysinfo.cpp

        private/plat/file/windows/file.cpp

		private/plat/plat_windows_errors.cpp
        )
    if(NOT WIN_UWP AND NOT MINGW64)
        target_sources ( CoffeeCore_Base PRIVATE
            private/plat/environment/windows/windows_startup.cpp
            )
    elseif(WIN_UWP)
        target_sources ( CoffeeCore_Base PRIVATE
            private/plat/environment/windows/windows_startup_uwp.cpp
            )
    endif()
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
    target_sources ( CoffeeCore_Base PRIVATE
        private/types/tdef/thread_types.cpp
        )
endif()

coffee_bundle_includes (
    ${STANDARDLIBS_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
    )
