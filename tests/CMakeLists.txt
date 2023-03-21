set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests )

add_subdirectory(cmd-api)
add_subdirectory(comp-app)
add_subdirectory(datastorage-api)
add_subdirectory(file-api)
add_subdirectory(graphics)
add_subdirectory(image-api)
add_subdirectory(math-logic)
add_subdirectory(profiling-api)
add_subdirectory(size-tests)
add_subdirectory(string-api)
add_subdirectory(sysinfo-api)
add_subdirectory(type-safe)

if(BUILD_ASIO)
    add_subdirectory(http_test)
endif()

if(NOT EMSCRIPTEN)
    add_subdirectory(thread-api)
endif()

if(NOT SKIP_LINKAGE_TEST)
    add_subdirectory(link-api)
endif()

# This needs different testing and setup
if(NOT ANDROID AND NOT WIN32)
    # Known to not work with:
    #  - Android (limitations in ant buildtool? thinks it's invalid)
    #  - Windows (just plain terrible filename encoding)
    add_subdirectory(packaging-api)
endif()

if(NOT SKIP_GRAPHIC_TESTS AND FALSE)
    add_subdirectory(window-api/basic)
    add_subdirectory(graphics/api-verify)
    add_subdirectory(graphics/self-test)
endif()

# This dummy target helps coverage see all the code,
#  for more accurate coverage info.
coffee_test (
    TARGET Unit_IncludeAll
    TITLE "Dummy_Target"

    SOURCES
    dummy.cpp

    LIBRARIES
    Core
    )

if(TARGET Blam)
    target_link_libraries ( Dummy_Target PRIVATE
        Blam
        )
endif()

if(TARGET Audio)
    target_link_libraries ( Dummy_Target PRIVATE
        Audio
        )
endif()

if(BUILD_GLEAM)
    target_link_libraries ( Dummy_Target
        PUBLIC
        GLeamCommon
        ComponentBundleSetup
        )
endif()
if(BUILD_ASSIMP)
    target_link_libraries ( Dummy_Target
        PUBLIC
        AssimpTools
        )
    target_compile_definitions ( Dummy_Target
        PUBLIC
        HAS_ASSIMP
        )
endif()
