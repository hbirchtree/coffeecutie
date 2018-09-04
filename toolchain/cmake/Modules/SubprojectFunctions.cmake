macro( INIT_SUBPROJECT )

    export ( PACKAGE ${PROJECT_NAME} )

    ############################################################################
    # Basic CMake configuration
    ############################################################################

    set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

    if(NOT WIN32)
        set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
    else()
        set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    endif()

    ############################################################################
    # Internal structure
    ############################################################################

    set ( COFFEE_DESKTOP_DIRECTORY
        "${CMAKE_SOURCE_DIR}/toolchain/desktop"
        )
    set ( CMAKE_PACKAGED_OUTPUT_PREFIX
        packaged
        )
    set ( COFFEE_PACKAGE_DIRECTORY 
        "${PROJECT_BINARY_DIR}/${CMAKE_PACKAGED_OUTPUT_PREFIX}"
        )
    set ( COFFEE_DEPLOY_DIRECTORY 
        "${PROJECT_BINARY_DIR}/deploy"
        )
    set ( COFFEE_SOURCE_TEMPLATE_DIR 
        "${CMAKE_SOURCE_DIR}/toolchain/internal/templates"
        )
    set ( COFFEE_SOURCE_TEMPLATE_DIRECTORY
        "${COFFEE_SOURCE_TEMPLATE_DIR}"
        )
    set ( COFFEE_CMAKE_TEMPLATE_DIR
        "${CMAKE_SOURCE_DIR}/toolchain/cmake/Templates"
        )

    ############################################################################
    # Platform-specific CMake modules
    ############################################################################

    if(ANDROID)
        set ( CMAKE_MODULE_PATH
            #        ${CMAKE_SOURCE_DIR}/cmake/Find_Android
            ${CMAKE_MODULE_PATH}
            )
    elseif(WIN32)
        set ( CMAKE_MODULE_PATH
            ${CMAKE_SOURCE_DIR}/cmake/Find_Windows
            ${CMAKE_MODULE_PATH}
            )
    endif()

    ############################################################################
    # CMake scripting paths
    ############################################################################

    set ( CMAKE_PREFIX_PATH
        ${COFFEE_ROOT_DIR}/share
        )

    set ( CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_SOURCE_DIR}/toolchain/cmake/Find
        ${CMAKE_SOURCE_DIR}/toolchain/cmake/Modules
        ${CMAKE_SOURCE_DIR}/toolchain/cmake/Packaging
        )

    ############################################################################
    # CMake helper scripts
    ############################################################################

    include ( BuildFlags )
    include ( BuildFunctions )

    ############################################################################
    # Output types
    ############################################################################

    option ( BUILD_LIBRARIES    "Build libraries"     ON  )
    option ( BUILD_BINARIES     "Build binaries"      ON  )
    option ( BUILD_UTILITIES    "Build tools"         ON  )
    option ( BUILD_TESTS        "Build unit tests"    ON  )
    option ( BUILD_EXAMPLES     "Build examples"      ON  )
    option ( BUILD_DOCS         "Build documentation" OFF )

    ############################################################################
    # Build modifiers
    # These impact the build in multiple ways
    ############################################################################

    option ( LOWFAT_BUILD "Build with minimal overhead" OFF )

    ############################################################################
    # Coverage and testing
    ############################################################################

    if(NOT WIN32)
        option ( BUILD_COVERAGE "Build with coverage testing"  OFF )

        option ( BUILD_ASAN     "Build with address sanitizer"            OFF )
        option ( BUILD_TSAN     "Build with thread sanitizer"             OFF )
        option ( BUILD_UBSAN    "Build with undefined behavior sanitizer" OFF )
    endif()

    # Enable coverage when requested
    if(BUILD_COVERAGE)
        include ( CodeCoverage )
        APPEND_COVERAGE_COMPILER_FLAGS()
    endif()

    if(BUILD_TESTS)
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR APPLE)
            option ( VALGRIND_MASSIF OFF )
            option ( VALGRIND_MEMCHECK OFF )
            option ( VALGRIND_CALLGRIND OFF )
            option ( VALGRIND_CACHEGRIND OFF )
        endif()

        include ( CTest )
        enable_testing()
    endif()

    # Coverage and sanitizers
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
            OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        if(BUILD_ASAN OR BUILD_TSAN OR BUILD_UBSAN)
            if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                message ( WARNING "Sanitizers should run in Debug mode!" )
            endif()

            set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer" )
        endif()

        if(BUILD_COVERAGE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set ( CMAKE_CXX_FLAGS
                "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping"
                )
        endif()

        if(BUILD_ASAN)
            set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" )
        endif()
        if(BUILD_TSAN)
            set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread" )
        endif()
        if(BUILD_UBSAN)
            set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined" )
        endif()

        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "" )
    endif()

    include ( WindowsPlatformDetect )

    ############################################################################
    # Packaging options
    ############################################################################

    # The multiple Linux packaging formats
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        option ( GENERATE_APPIMAGE "Generate AppImage binaries" OFF )
        option ( GENERATE_SNAPPY   "Generate Snap packages"     OFF )
        option ( GENERATE_FLATPAK  "Generate Flatpaks"          OFF )
    endif()

    # When building for Emscripten, we output HTML targets
    if(EMSCRIPTEN)
        option ( GENERATE_HTML "Generate HTML wrapper" ON )

        if(GENERATE_HTML)
            set ( CMAKE_EXECUTABLE_SUFFIX ".html" )
        endif()
    endif()
endmacro()

macro( INIT_PROJECT )
    init_subproject()
endmacro()

macro( END_SUBPROJECT )
    if(BUILD_LIBRARIES)
        generate_findscript()
    endif()
endmacro()

macro( END_PROJECT )
    end_subproject()
endmacro()

################################################################################
# Helper for importing Coffee libraries
################################################################################

macro( IMPORT_COFFEE_LIB LIBNAME DEPENDENCY )
    if( ${${LIBNAME}_DIR} )
        include( ${${LIBNAME}_DIR}/${LIBNAME}Config.cmake )
        return()
    endif()
    if( ${COFFEE_ROOT_DIR} )
        include( ${COFFEE_ROOT_DIR}/share/${LIBNAME}Config.cmake )
        return()
    endif()
    find_package( ${LIBNAME} ${DEPENDENCY} )
endmacro()
