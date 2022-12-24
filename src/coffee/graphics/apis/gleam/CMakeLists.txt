option ( BUILD_DESKTOP_GL "Override for building Desktop GL on f.ex. Android" OFF )

option ( BUILD_GLES_20 "Build against OpenGL ES 2.0 (mutually exclusive)" OFF )
option ( BUILD_GLES "Build library targeting OpenGL ES 3.0+" OFF )
option ( BUILD_GLES_31 "Build with support for OpenGL ES 3.1 (Only activates when BUILD_GLES is on)" ON )
option ( BUILD_GLES_32 "Build with support for OpenGL ES 3.2 (Only activates when BUILD_GLES is on)" ON )

if(WIN32 AND WIN_UWP)
    option ( BUILD_ANGLE "Build on ANGLE as backend for OpenGL ES" ON )
endif()

if (
    (NOT IOS AND
     NOT ANDROID AND
     NOT EMSCRIPTEN AND
     NOT (WIN32 AND WIN_UWP)) OR
     BUILD_DESKTOP_GL
    )
    add_library ( GLVersion_Dynamic INTERFACE )
    if (NOT APPLE)
        target_compile_definitions ( GLVersion_Dynamic INTERFACE
            -DGLEAM_USE_CORE
            -DGLEAM_MAX_VERSION=0x460
            -DGLEAM_MAX_VERSION_ES=0
            )
    else()
        target_compile_definitions ( GLVersion_Dynamic INTERFACE
            -DGLEAM_USE_CORE
            -DGLEAM_MAX_VERSION=0x410
            -DGLEAM_MAX_VERSION_ES=0
            )
    endif()
    target_link_libraries ( GLVersion_Dynamic INTERFACE GLAD_Core )
    add_export ( GLVersion_Dynamic "" )
endif()
if(IOS OR ANDROID OR EMSCRIPTEN OR BUILD_GLES)
    add_library ( GLVersion_ES INTERFACE )
    if(EMSCRIPTEN OR IOS)
        target_compile_definitions ( GLVersion_ES INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_USE_LINKED
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x300
            )
    else()
        set ( MAX_VERSION 0x300 )
        if(BUILD_GLES_31)
            set ( MAX_VERSION 0x310 )
        endif()
        if(BUILD_GLES_32)
            set ( MAX_VERSION 0x320 )
        endif()
        target_compile_definitions ( GLVersion_ES INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_USE_LINKED
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=${MAX_VERSION}
            )
    endif()
    add_export ( GLVersion_ES "" )

    add_library ( GLVersion_ES2 INTERFACE )
    target_compile_definitions ( GLVersion_ES2 INTERFACE
        -DGLEAM_USE_ES
        -DGLEAM_USE_LINKED
        -DGLEAM_MAX_VERSION=0
        -DGLEAM_MAX_VERSION_ES=0x200
        -DGLEAM_RESTRICT_ES=0x200
        )
    add_export ( GLVersion_ES2 "" )
    if (NOT APPLE)
        add_library ( GLVersion_ESDynamic INTERFACE )
        target_compile_definitions ( GLVersion_ESDynamic INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x320
            )
        target_link_libraries ( GLVersion_ESDynamic INTERFACE GLAD_ES )
        add_export ( GLVersion_ESDynamic "" )
        add_library ( GLVersion_ES2Dynamic INTERFACE )
        target_compile_definitions ( GLVersion_ES2Dynamic INTERFACE
            -DGLEAM_USE_ES
            -DGLEAM_MAX_VERSION=0
            -DGLEAM_MAX_VERSION_ES=0x200
            -DGLEAM_RESTRICT_ES=0x200
            )
        target_link_libraries ( GLVersion_ES2Dynamic INTERFACE GLAD_ES )
        add_export ( GLVersion_ES2Dynamic "" )
    endif()
endif()

function ( GEN_GLEAM_TARGET GLVERSION SUFFIX )
    coffee_library (
        TARGET GLeamCommon_${SUFFIX}

        SOURCES
        private/glw.cpp
    #    private/levels/shared/gl_shared_debug.cpp

        LIBRARIES
        Core
        Image
        GraphicsAPI
        GraphicsInterface
        ${GLVERSION}

        HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        $<INSTALL_INTERFACE:include>
        )


    if(BUILD_GLEAM)
        coffee_library (
            TARGET GLeamRHI_${SUFFIX}
            SOURCES

            private/rhi/rhi.cpp

            LIBRARIES
            GLeamCommon_${SUFFIX}
            ComponentApp

            HEADER_DIRS
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            )
    endif()

    if (${GLVERSION} MATCHES "_Dynamic")
        target_include_directories ( GLeamCommon_${SUFFIX} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gl_include>
            )
    elseif (${GLVERSION} MATCHES "_ES2?Dynamic")
        target_include_directories ( GLeamCommon_${SUFFIX} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gles_include>
            )
    endif()

    if(APPLE AND IOS AND ${GLVERSION} MATCHES "_ES")
        target_compile_definitions ( GLeamCommon_${SUFFIX} PUBLIC
            -DGLES_SILENCE_DEPRECATION=1
            )
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
            CEAGL
            "-framework OpenGLES"
            )
        target_include_directories ( GLeamCommon_${SUFFIX} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/coffee/windowing/private/graphics/ceagl/include>
            )
    elseif(BUILD_ANGLE AND ${GLVERSION} MATCHES "_ES")
        find_package( ANGLE CONFIG REQUIRED )
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC AngleEGL AngleGLESv2 )
    elseif(APPLE AND ${GLVERSION} MATCHES "_Dynamic")
        target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC "-framework OpenGL" )
    elseif(${GLVERSION} MATCHES "_ES")
        if(ANDROID AND NOT BUILD_GLES_20)
            # On Android, GLES 3.x is used through a different library
            target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
                GLESv3
                )
        elseif(EMSCRIPTEN)
            target_link_options ( GLeamCommon_${SUFFIX} PUBLIC
                -sMAX_WEBGL_VERSION=2
                -sFULL_ES3
                )
        else()
            find_library ( GLESV2_LIB GLESv2 )

            # Elsewhere, just use GLESv2
            target_link_libraries ( GLeamCommon_${SUFFIX} PUBLIC
                ${GLESV2_LIB}
                )
        endif()
    endif()
endfunction()

foreach ( VER Dynamic ES ES2 ESDynamic ES2Dynamic )
    if (NOT TARGET GLVersion_${VER} )
        continue()
    endif()
    gen_gleam_target ( GLVersion_${VER} ${VER} )
endforeach()

if ( TARGET GLVersion_Dynamic )
    coffee_bundle (
        DESTINATION ""
        HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/gl_include/glad"
        )
endif()
if ( TARGET GLVersion_ESDynamic OR TARGET GLVersion_ES2Dynamic )
    coffee_bundle (
        DESTINATION ""
        HEADER_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/gles_include/glad_es"
        )
endif()
