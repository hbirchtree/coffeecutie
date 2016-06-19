if(ANDROID)
    add_subdirectory(android)
endif()

if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(swig)
endif()
