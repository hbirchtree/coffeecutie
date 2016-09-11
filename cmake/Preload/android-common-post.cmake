if(ANDROID_NATIVE_API_LEVEL LESS 17)
    message ( "-- Disabling GLEAM_RHI, API ${ANDROID_NATIVE_API_LEVEL} doesn't support it" )
    set ( COFFEE_BUILD_GLEAM_RHI OFF )
endif()
