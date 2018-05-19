macro( INIT_SUBPROJECT )
    set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

    if(NOT WIN32)
        set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
    else()
        set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    endif()

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

    set ( CMAKE_PREFIX_PATH
        ${COFFEE_ROOT_DIR}/share
        )

    set ( CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_SOURCE_DIR}/toolchain/cmake/Find
        ${CMAKE_SOURCE_DIR}/toolchain/cmake/Modules
        ${CMAKE_SOURCE_DIR}/toolchain/cmake/Packaging
        )

    include ( BuildOptions )
    include ( BuildFlags )
    include ( BuildFunctions )
    include ( PackagingOptions )
    include ( TestingOptions )

    option ( BUILD_TESTS "Build unit tests" ON )

    if(LOWFAT_BUILD)
        add_definitions ( -DCOFFEE_LOWFAT )
    endif()
 
    # Enable coverage when requested
    if(BUILD_COVERAGE)
        include ( CodeCoverage )
        APPEND_COVERAGE_COMPILER_FLAGS()
    endif()

    # When building for Emscripten, we output HTML targets
    if(COFFEE_GENERATE_HTML)
        set ( CMAKE_EXECUTABLE_SUFFIX ".html" )
    endif()

    if(BUILD_TESTS)
        include ( CTest ) 
    endif()
endmacro()

macro( INIT_PROJECT )
    init_subproject()
endmacro()

macro( END_SUBPROJECT )
    generate_findscript()
endmacro()

macro( END_PROJECT )
    end_subproject()
endmacro()

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
