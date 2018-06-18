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
    ${COFFEE_EXT_LIBRARY_DIR}/tinyxml2/tinyxml2.cpp

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
if(BUILD_SDL2
        AND NOT POWERPC AND NOT IOS
        AND (ANDROID OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR WIN32)
        )
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
    list ( APPEND CoffeeCore_SRCS
        ${NDK_HELPER_SOURCES}
        ${CPUFEATURES_SOURCES}
        )
    list ( APPEND CoffeeCore_AppLoader_SRCS
        platform/and_main.cpp
        ${ANDROID_GLUE_SOURCES}
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
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT ANDROID)
    list ( APPEND CoffeeCore_SRCS
        private/plat/environment/linux/environment.cpp
        private/plat/file/linux/file.cpp
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
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE OR ANDROID)
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
    if(UNWIND_FOUND)
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

coffee_library (
    TARGET CoffeeXML
    SOURCES ${CoffeeXML_SRCS}
    )

coffee_library (
    TARGET CoffeeCore_Base
    SOURCES
    ${CoffeeCore_SRCS}
    ${CoffeeFile_SRCS}
    ${CoffeeEventAppWrap_SRCS}
    ${CoffeeSysInfo_SRCS}
    ${CoffeeCompression_SRCS}
    ${CoffeePlatformData_SRCS}
    ${CoffeeProfiler_SRCS}

    LIBRARIES
    ${STANDARDLIBS_LIBRARIES}
    CoffeeXML

    HEADER_DIRS
    ${CMAKE_SOURCE_DIR}/include/coffee/core
    )
coffee_library (
    TARGET CoffeeCore_Application
    SOURCES
    ${CoffeeCore_AppLoader_SRCS}
    ${CoffeeCore_Bootstrap_SRCS}
    LIBRARIES CoffeeCore_Base
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/core
    )

coffee_library (
    TARGET CoffeeINI
    LIBRARIES CoffeeCore_Base
    SOURCES ${CoffeeINI_SRCS}
    )
coffee_library (
    TARGET CoffeeTesting
    SOURCES ${CoffeeTesting_SRCS}
    LIBRARIES CoffeeCore_Base
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/core/unit_tests
    )

target_compile_definitions ( CoffeeCore_Base PUBLIC
    -DCOFFEE_BUILD_STRING="${COFFEE_BUILD_STRING}"
    )

if(LOWFAT_BUILD)
    target_compile_definitions (  CoffeeCore_Base PUBLIC
        -DCOFFEE_LOWFAT
        )
endif()

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

target_include_directories ( CoffeeCore_Base
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
    $<INSTALL_INTERFACE:include>
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

if(BUILD_SWIG_BINDING)
    coffee_library (
        TARGET CoffeeCoreLoadable
        LINKAGE SHARED
        SOURCES ${CoffeeCore_SRCS}
        LIBRARIES ${STANDARDLIBS_LIBRARIES}
        HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/core
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
    if(NOT TARGET SDL2)
        find_package ( SDL2 QUIET )
    endif()
    target_link_libraries ( CoffeeCore_Base
        PRIVATE
        SDL2
        )
endif()

if(ANDROID)
    target_compile_definitions ( CoffeeCore_Base PUBLIC
        -DANDROID_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
        )

    target_include_directories ( CoffeeCore_Base PRIVATE
        ${ANDROID_GLUE_INCLUDE_DIR}
        ${NDK_HELPER_INCLUDE_DIR}
        ${CPUFEATURES_INCLUDE_DIR}
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/bindings/android/include>
        )
    target_include_directories ( CoffeeCore_Application
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
