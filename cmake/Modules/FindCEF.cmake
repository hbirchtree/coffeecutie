# Find the CEF librar(ies/y)
#
# Defines:
#   CEF_FOUND
#   CEF_LIBRARIES
#   CEF_INCLUDE_DIR

# Inputs:
#   CEF_ROOT_DIR

set(CEF_ROOT_DIR "${CEF_ROOT_DIR}" CACHE PATH "Directory to search for CEF")

find_path ( CEF_INCLUDE_DIR
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

if (NOT CEF_LIBRARY_DEBUG AND NOT CEF_LIBRARY_RELEASE )
    message (FATAL_ERROR "Failed to locate CEF library")
endif()
if (NOT CEF_INCLUDE_DIR)
    message (FATAL_ERROR "Failed to locate CEF include directory")
endif()

set ( CEF_LIBRARIES ${CEF_LIBRARY_WRAPPER} )

if( CEF_LIBRARY_DEBUG AND CMAKE_BUILD_TYPE EQUAL Debug )
    list ( APPEND CEF_LIBRARIES ${CEF_LIBRARY_DEBUG} )
elseif( CEF_LIBRARY_RELEASE )
    list ( APPEND CEF_LIBRARIES ${CEF_LIBRARY_RELEASE} )
endif()

set ( CEF_FOUND FOUND )

mark_as_advanced ( CEF_LIBRARIES CEF_INCLUDE_DIR )
