set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests )

add_subdirectory(cmd-api)
add_subdirectory(datastorage-api)
add_subdirectory(file-api)
add_subdirectory(function-tests)
add_subdirectory(math-logic)
add_subdirectory(profiling-api)
add_subdirectory(size-tests)
add_subdirectory(string-api)
add_subdirectory(sysinfo-api)
add_subdirectory(graphics)

if(NOT EMSCRIPTEN)
    add_subdirectory(thread-api)
endif()

if(NOT SKIP_LINKAGE_TEST)
    add_subdirectory(link-api)
endif()
#if(COFFEE_BUILD_OPENVR AND NOT SKIP_OPENVR_TEST)
#    add_subdirectory(vr-api)
#endif()

# This needs different testing and setup
if(NOT ANDROID AND NOT WIN32)
    # Known to not work with:
    #  - Android (limitations in ant buildtool? thinks it's invalid)
    #  - Windows (just plain terrible filename encoding)
    add_subdirectory(packaging-api)
endif()
