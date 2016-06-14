# This is where we put the Android-specific shim functions (JNI and etc.)

find_package ( AndroidGlue )

set ( AndroidCore_SRCS
        stub.cpp
#        static.cpp
        )

if(NOT ANDROID_USE_SDL2_LAUNCH)
    list (APPEND AndroidCore_SRCS
            ${ANDROID_GLUE_SOURCES} ${COFFEE_ANDROID_MAIN}
            )
endif()

coffee_add_library ( AndroidCore
        "${AndroidCore_SRCS}"
         )
