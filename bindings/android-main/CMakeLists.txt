# This is where we put the Android-specific shim functions (JNI and etc.)
if(ANDROID)
    find_package ( SDL2main REQUIRED )

    coffee_add_library ( AndroidCore static.cpp )
endif()
