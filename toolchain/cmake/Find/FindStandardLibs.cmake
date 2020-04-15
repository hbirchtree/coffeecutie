if(NOT TARGET StandardLibs)
    # Some variables
    set ( CORE_INCLUDE_DIR )
    set ( CORE_EXTRA_LIBRARIES )

    add_library ( StandardLibs INTERFACE )

endif()

message ( STATUS "Standardlibs in use: ${CORE_EXTRA_LIBRARIES}" )
message ( STATUS "Standard includes in use: ${CORE_INCLUDE_DIR}" )
