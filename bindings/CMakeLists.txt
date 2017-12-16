if(ANDROID)
    # Contains Android-specific code, mostly for the NativeActivity implementation
    add_subdirectory(android)
endif()

if(COFFEE_BUILD_SWIG_BINDING)
    # Builds a C-like wrapper library for SWIG to embed
    add_subdirectory(swig)
endif()

if(NOT WIN32)
    add_subdirectory ( c-binding )
endif
