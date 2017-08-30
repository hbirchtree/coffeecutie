macro( INIT_SUBPROJECT )
    set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

    set ( COFFEE_DESKTOP_DIRECTORY "${CMAKE_SOURCE_DIR}/desktop" )
    set ( CMAKE_PACKAGED_OUTPUT_PREFIX packaged )
    set ( COFFEE_PACKAGE_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_PACKAGED_OUTPUT_PREFIX}" )
    set ( COFFEE_DEPLOY_DIRECTORY "${PROJECT_BINARY_DIR}/deploy" )
    set ( COFFEE_SOURCE_TEMPLATE_DIRECTORY "${CMAKE_SOURCE_DIR}/internal/templates" )

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
        ${CMAKE_SOURCE_DIR}/cmake/Find
        ${CMAKE_SOURCE_DIR}/cmake/Modules
        ${CMAKE_SOURCE_DIR}/cmake/Packaging
        )

    if(NOT WIN32)
        set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
    else()
        set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
    endif()

    include ( BuildOptions )
    include ( BuildFlags )
    include ( BuildFunctions )
    include ( PackagingOptions )
    include ( TestingOptions )

    option ( BUILD_TESTS "Build unit tests" ON )
endmacro()

macro( END_SUBPROJECT )
    generate_findscript()
endmacro()

macro( IMPORT_COFFEE_LIB LIBNAME DEPENDENCY )
    find_package( ${LIBNAME} ${DEPENDENCY} )
    if( ${${LIBNAME}_DIR} )
        include( ${${LIBNAME}_DIR}/${LIBNAME}Config.cmake )
    endif()
endmacro()
