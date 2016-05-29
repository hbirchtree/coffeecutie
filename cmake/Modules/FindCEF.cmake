# Find the CEF librar(ies/y)
#
# Defines:
#   CEF_FOUND
#   CEF_LIBRARIES
#   CEF_INCLUDE_DIR

# Inputs:
#   CEF_ROOT_DIR

set ( CEF_ROOT_DIR "${CEF_ROOT_DIR}" CACHE PATH "Directory to search for CEF")
set ( CEF_LIBRARIES "" CACHE FILEPATH "Library file for CEF" )
set ( CEF_INCLUDE_DIR "" CACHE PATH "Include dir for CEF" )

find_path ( CEF_INCLUDE_DIR_TMP
    # Which headers to search for

    include/cef_app.h
    include/cef_base.h
    include/cef_browser.h
    include/cef_render_handler.h

    # Where to search for them
    PATHS
    ${CEF_ROOT_DIR}
    )

find_library ( CEF_LIBRARY_DEBUG
    # Library name
    cef

    PATHS
    ${CEF_ROOT_DIR}/Debug
    )

find_library ( CEF_LIBRARY_RELEASE
    # Library name
    cef

    PATHS
    ${CEF_ROOT_DIR}/Release
    )

find_library ( CEF_LIBRARY_WRAPPER
    #Fucking hell.
    cef_dll_wrapper

    PATHS
    ${CEF_ROOT_DIR}/libcef_dll
    )

if( CEF_LIBRARY_DEBUG AND CMAKE_BUILD_TYPE EQUAL Debug )
    list ( APPEND CEF_LIBRARIES ${CEF_LIBRARY_DEBUG} )
elseif( CEF_LIBRARY_RELEASE )
    list ( APPEND CEF_LIBRARIES ${CEF_LIBRARY_RELEASE} )
endif()

if ( CEF_INCLUDE_DIR_TMP )
    set ( CEF_INCLUDE_DIR "${CEF_INCLUDE_DIR_TMP}" )
endif()

if ( CEF_INCLUDE_DIR AND CEF_LIBRARIES AND CEF_LIBRARY_WRAPPER )
    list ( APPEND CEF_LIBRARIES ${CEF_LIBRARY_WRAPPER} )
    set ( CEF_FOUND FOUND )
endif()

mark_as_advanced ( CEF_LIBRARIES CEF_INCLUDE_DIR )
