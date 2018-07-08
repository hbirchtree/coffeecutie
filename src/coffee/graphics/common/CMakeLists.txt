coffee_library (
    TARGET CoffeeGraphicsAPI
    SOURCES private/query/gpu_query.cpp
    )

target_link_libraries ( CoffeeGraphicsAPI PRIVATE
    CoffeeCore
    )

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package( NvidiaML QUIET )

find_package( LibDRM QUIET )

if(NVIDIAML_FOUND)
    coffee_library (
        TARGET CoffeeNVIDIAQuery
        LINKAGE SHARED
        SOURCES
        private/query/nvidia_query.cpp

        LIBRARIES
        ${NVIDIAML_LIBRARIES}
        CoffeeCore
        )

    target_include_directories( CoffeeNVIDIAQuery PRIVATE
        $<BUILD_INTERFACE:${NVIDIAML_INCLUDE_DIR}>
        )
endif()

if(LIBDRM_FOUND AND FALSE)
    coffee_library (
        TARGET CoffeeLibDRMQuery
        LINKAGE SHARED
        SOURCES
        private/query/libdrm_query.cpp
        private/query/libdrm_query.c

        LIBRARIES
        ${LIBDRM_LIBRARIES}
        CoffeeCore
        )

    target_include_directories( CoffeeLibDRMQuery PRIVATE
        $<BUILD_INTERFACE:${LIBDRM_INCLUDE_DIR}>
        )
endif()

if(BUILD_OPENGL AND BUILD_GLEAM AND NOT EMSCRIPTEN AND NOT ANDROID AND NOT APPLE)
    coffee_library (
        TARGET CoffeeOpenGLQuery
        LINKAGE SHARED
        SOURCES private/query/opengl_query.cpp

        LIBRARIES
        CoffeeCore
        CoffeeBinding_GLAD
        CoffeeGLeamCommon
        )
endif()
