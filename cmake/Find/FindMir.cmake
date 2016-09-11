set ( MIR_SEARCH_PATHS
    /usr
    /usr/local
    )

find_path ( MIR_INCLUDE_DIR
    cursors.h event.h mir_input_device.h
    PATH_SUFFIXES include include/mirclient/mir_toolkit
    PATHS ${MIR_SEARCH_PATHS}
    )

find_library ( MIR_CLIENT_LIB_TMP
    NAMES mirclient
    PATH_SUFFIXES lib
    PATHS ${MIR_SEARCH_PATHS}
    )

if(NOT MIR_CLIENT_LIB_TMP)
    set ( MIR_LIBRARIES "${MIR_CLIENT_LIB_TMP}"
        CACHE STRING "" )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Mir REQUIRED_VARS MIR_LIBRARIES MIR_INCLUDE_DIR)
