add_subdirectory(android-main)

if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(swig)
endif()
