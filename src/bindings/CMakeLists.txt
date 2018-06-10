option ( BUILD_SWIG_BINDING "Build SWIG language bindings"  OFF )
option ( BUILD_CBINDING     "Build C bindings"              OFF )

if(BUILD_SWIG_BINDING)
    # Builds a C-like wrapper library for SWIG to embed
    add_subdirectory(swig)
endif()

if(NOT WIN32 AND BUILD_CBINDING)
    add_subdirectory ( c-binding )
endif()
