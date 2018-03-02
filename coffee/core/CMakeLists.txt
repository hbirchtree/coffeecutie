set ( CORE_INC_DIR ${COFFEE_INCLUDE_DIR}/core/ )

set ( CoffeeCore_PCH
    ${CORE_INC_DIR}/plat/plat_environment.h
    ${CORE_INC_DIR}/plat/plat_file.h
    ${CORE_INC_DIR}/plat/plat_memory.h

    ${CORE_INC_DIR}/platform_data.h

    ${CORE_INC_DIR}/CApplication
    ${CORE_INC_DIR}/CArgParser
    ${CORE_INC_DIR}/CFiles
    ${CORE_INC_DIR}/CIniParser
    ${CORE_INC_DIR}/CInput
    ${CORE_INC_DIR}/CJSONParser
    ${CORE_INC_DIR}/CMD
    ${CORE_INC_DIR}/CPlatform
    ${CORE_INC_DIR}/CProfiling
    ${CORE_INC_DIR}/CRegex
    ${CORE_INC_DIR}/CTiming
    ${CORE_INC_DIR}/CXmlParser
    )

set ( CoffeeCore_SRCS

    private/extern_storage.cpp
    private/coffee_signals.cpp
    private/argument_handling.cpp

    private/application_data.cpp

    # Internal object / debug functions
    private/base/cdebug.cpp
    private/base/cdisplay.cpp
    private/base/cobject.cpp
    private/base/cregex.cpp
    private/base/debugprinter.cpp

    private/base/strings/extensionresolvers.cpp
    private/base/strings/information_extensions.cpp
    private/base/strings/vector_print_extensions.cpp

    private/types/cdef/infotypes.cpp
    private/types/tdef/thread_types.cpp

    # Terminal functions
    private/terminal/table-print.cpp

    # Isolation of platform libraries
    #private/plat/environment_wall.cpp

    # Internal platform layer
    private/plat/environment/argument_parse.cpp
    private/plat/environment/shared_func.cpp

    private/plat/memory/cmd_interface.cpp
    private/plat/memory/string_ops.cpp

    # Task queues, for easier time scheduling
    private/task_queue/task.cpp

    private/datastorage/binary/virtualfs.cpp
    )

set ( CoffeeXML_SRCS
    # We slap tinyXML2 in here, just cause
    ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp

    )

set ( CoffeeJSON_SRCS
    # There's nothing here...
    )

set ( CoffeeINI_SRCS
    private/datastorage/text/ini/ciniparser.cpp
    )

set ( CoffeeTesting_SRCS
    # Unit testing
    private/unit_tests/framework.cpp
    )

set ( CoffeeFile_SRCS
    private/plat/file/file_common.cpp
    private/base/cfiles.cpp
    private/base/url.cpp

    # Storage stuff, may be externalized
    private/coffee_saving.cpp
    )

set ( CoffeeCompression_SRCS
    # Compression / data storage
    private/datastorage/compression/libz.cpp
    )

set ( CoffeeSysInfo_SRCS
    # All of this is platform-specific
    )

set ( CoffeeEventAppWrap_SRCS
    private/base/eventapplication_wrapper.cpp
    )
set ( CoffeePlatformData_SRCS
    private/platform_data.cpp
    )

set ( CoffeeCore_AppLoader_SRCS
    private/plat/environment/application_start.cpp
    )

set ( CoffeeCore_Bootstrap_SRCS
    private/coffee.cpp
    )

set ( CoffeeProfiler_SRCS
    # Profiling
    private/profiler/profiling-export.cpp
    )

# Platform-specific code, put it here

# SDL power-management
if(COFFEE_BUILD_SDL2
        AND NOT POWERPC
        AND NOT IOS
        AND (ANDROID
            OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux"
            OR APPLE
            OR WIN32))
    set ( SDL_POWER_PLUGIN_ENABLED ON )
    list ( APPEND CoffeeSysInfo_SRCS
	private/plat/environment/sdlpowerinfo.cpp
	)
endif()

# Android-specific code
if(ANDROID)
    find_package( AndroidGlue REQUIRED )

    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/android/environment.cpp
        private/plat/sensor/android/sensor.cpp
        private/plat/file/linux/file.cpp

        )
    list ( APPEND CoffeeCore_Bootstrap_SRCS
        platform/and_main.cpp
        ${ANDROID_GLUE_SOURCES}
        ${NDK_HELPER_SOURCES}
        ${CPUFEATURES_SOURCES}
        )

    set_source_files_properties (
        ${ANDROID_GLUE_SOURCES}
        PROPERTIES
        COMPILE_DEFINITIONS "NDEBUG"
        )

    list ( APPEND CoffeeFile_SRCS
        private/plat/file/android/file.cpp
        )
    list ( APPEND CoffeeSysInfo_SRCS
        # For reading process information
        private/plat/environment/linux/process.cpp
        private/plat/environment/linux/sysinfo.cpp
        private/plat/environment/android/sysinfo.cpp
        )
endif()

# Linux-specific code
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID AND NOT NACL)
    list ( APPEND CoffeeCore_SRCS
	private/plat/environment/linux/environment.cpp
        private/plat/file/linux/file.cpp
        )
    list ( APPEND CoffeeFile_SRCS
        )
    list ( APPEND CoffeeSysInfo_SRCS
        private/plat/environment/linux/process.cpp
        private/plat/environment/linux/sysinfo.cpp
        )
endif()

if(EMSCRIPTEN)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/unix/environment.cpp
        private/plat/environment/emscripten/sysinfo.cpp
        )
endif()

if(NACL)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/native-client/sysinfo.cpp
        )
endif()

# Mac OS X-specific code
if(APPLE)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/osx/sysinfo.cpp
        private/plat/environment/osx/environment.cpp
        private/plat/environment/osx/system_interface.m
        )
    list ( APPEND CoffeeFile_SRCS
        private/plat/file/osx/file.cpp
        )

endif()

# Windows-specific code
if(WIN32)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/windows/environment.cpp
        private/plat/environment/windows/sysinfo.cpp
        )
    list ( APPEND CoffeeFile_SRCS
        private/plat/file/windows/file.cpp
        )

#	set_source_files_properties ( private/plat/file/windows/file.cpp
#		PROPERTIES
#		COMPILE_FLAGS "/ZW"
#		)
        if(NOT WIN_UWP AND NOT MINGW64)
		list ( APPEND CoffeeCore_SRCS
			private/plat/environment/windows/windows_startup.cpp
			)
        elseif(WIN_UWP)
		list ( APPEND CoffeeCore_SRCS
			private/plat/environment/windows/windows_startup_uwp.cpp
			)
	endif()
endif()

if(RASPBERRY)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/raspberry/sysinfo.cpp
        )
endif()

# POSIX code
if(NOT NACL AND (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR ANDROID))
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/unix/environment.cpp
        private/plat/environment/unix/stacktrace.cpp
        private/plat/linking/unix/loader.cpp
        private/plat/file/unix/posix_shared.cpp
        )
    list ( APPEND CoffeeFile_SRCS
        private/plat/file/unix/file.cpp
        )

    find_package( Unwind QUIET )
    if(LIBUNWIND_FOUND)
        add_definitions(
            -DLIBUNWIND_ENABLED
            )
    endif()
endif()

if(MAEMO)
    list ( APPEND CoffeeCore_SRCS
        private/plat/sensor/maemo/sensors.cpp
        )
endif()

find_package ( StandardLibs REQUIRED )

coffee_add_library ( CoffeeXML
    "${CoffeeXML_SRCS}"
    ""
    ""
    )

coffee_add_library ( CoffeeCore_Base
    "${CoffeeCore_SRCS};${CoffeeFile_SRCS};${CoffeeEventAppWrap_SRCS};${CoffeeSysInfo_SRCS};${CoffeeCompression_SRCS}"
    "${STANDARDLIBS_LIBRARIES};CoffeeXML"
    "${CMAKE_SOURCE_DIR}/include/coffee/core"
    )
coffee_add_library ( CoffeeCore_Application
    "${CoffeeCore_AppLoader_SRCS};${CoffeeCore_Bootstrap_SRCS};${CoffeeProfiler_SRCS};${CoffeePlatformData_SRCS}"
    "CoffeeCore_Base"
    "${CMAKE_SOURCE_DIR}/include/coffee/core"
    )
#coffee_add_library ( CoffeeCore_AppLoader
#    ""
#    "CoffeeCore_Base;"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core"
#    )

#coffee_add_library ( CoffeeCore_Bootstrap
#    ""
#    "CoffeeCore_Base;"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core"
#    )

coffee_add_library ( CoffeeINI
    "${CoffeeINI_SRCS}"
    ""
    ""
    )
coffee_add_library ( CoffeeTesting
    "${CoffeeTesting_SRCS}"
    "CoffeeCore_Base"
    "${CMAKE_SOURCE_DIR}/include/coffee/core/unit_tests"
    )

#coffee_add_library ( CoffeeFile
#    ""
#    "CoffeeCore_Base;CoffeeEventAppWrap"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core/"
#    )
#coffee_add_library ( CoffeeSysInfo
#    ""
#    "CoffeeCore_Base"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core"
#    )
#coffee_add_library ( CoffeeEventAppWrap
#    ""
#    "CoffeeCore_Base"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core"
#    )
#coffee_add_library ( CoffeePlatformData
#    ""
#    "CoffeeCore_Base;CoffeeSysInfo"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core"
#    )
#coffee_add_library ( CoffeeProfiler
#    ""
#    "CoffeeCore_Base;CoffeePlatformData"
#    "${CMAKE_SOURCE_DIR}/include/coffee/core"
#    )

add_library ( CoffeeCore INTERFACE )

install(
    TARGETS CoffeeCore
    EXPORT ${PROJECT_NAME}
    )

register_library ( CoffeeCore "${CMAKE_SOURCE_DIR}/include/coffee/core" )

target_link_libraries ( CoffeeCore
    INTERFACE
    CoffeeCore_Base
    )

find_package ( RapidJson REQUIRED )
find_package ( TinyXML2 REQUIRED )

target_include_directories ( CoffeeCore_Base
    SYSTEM PUBLIC
    $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${TINYXML2_INCLUDE_DIR}>
    )

target_include_directories ( CoffeeCore_Base
    PUBLIC
    $<INSTALL_INTERFACE:include/>
    $<INSTALL_INTERFACE:include/tinyxml2/>
    ${STANDARDLIBS_INCLUDE_DIR}
    )

if(COFFEE_BUILD_SWIG_BINDING)


    coffee_add_elibrary ( CoffeeCoreLoadable SHARED
        "${CoffeeCore_SRCS}"
        "${STANDARDLIBS_LIBRARIES}"
        "${CMAKE_SOURCE_DIR}/include/coffee/core"
        )
    target_compile_definitions ( CoffeeCoreLoadable
        PRIVATE
        -DCOFFEE_LOADABLE_LIBRARY
        )
    target_include_directories ( CoffeeCoreLoadable
        PUBLIC
        $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${TINYXML2_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include/>
        $<INSTALL_INTERFACE:include/tinyxml2/>

        ${STANDARDLIBS_INCLUDE_DIR}
        )
endif()

if(SDL_POWER_PLUGIN_ENABLED)
    find_package ( SDL2 QUIET )
    target_link_libraries ( CoffeeCore_Base
        PRIVATE
        ${SDL2_LIBRARY}
        )
endif()

if(ANDROID)
    target_include_directories ( CoffeeCore_Base
        PRIVATE
        ${ANDROID_GLUE_INCLUDE_DIR}
        ${NDK_HELPER_INCLUDE_DIR}
        ${CPUFEATURES_INCLUDE_DIR}
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/bindings/android/include>
        )
endif()

coffee_bundle_includes (
    ${STANDARDLIBS_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
    )
