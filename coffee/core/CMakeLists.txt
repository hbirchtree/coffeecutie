find_package(glbinding)

set ( CoffeeCore_SRCS

        coffee.cpp
        base/counter.cpp
        base/cdebug.cpp
        base/cdisplay.cpp
        base/cfiles.cpp
        base/cobject.cpp
        base/cregex.cpp

        base/cdrendererbase.cpp

        context/cglbindingrenderer.cpp

        display/csdl2renderer.cpp
        display/coffeesdl2/sdl2eventhandlers.cpp
        display/coffeesdl2/sdl2helpers.cpp

        functional/cqueuerunner.cpp

        graphics/cgraphicsdata.cpp
        graphics/cgraphics_quirks.cpp

        graphics/opengl_glbinding/cbuffer.cpp
        graphics/opengl_glbinding/cshader.cpp
        graphics/opengl_glbinding/cframebuffer.cpp
        graphics/opengl_glbinding/cgraphicswrappers.cpp
        graphics/opengl_glbinding/cvertexarrayobject.cpp
        graphics/opengl_glbinding/ctexture.cpp
        graphics/opengl_glbinding/ctexture_dxtc.cpp
        graphics/opengl_glbinding/crenderfence.cpp
        graphics/opengl_glbinding/glbinding.cpp
        graphics/opengl_glbinding/copengl_types.cpp
        graphics/opengl_glbinding/ctexture_storage.cpp
        graphics/opengl_glbinding/ctexture_sampler.cpp
        graphics/opengl_glbinding/ctexture_pbo.cpp
        graphics/opengl_glbinding/cuniformtypes.cpp

        jobs/ctasksystem.cpp

        unit_tests/data_types.cpp

        input/cinputfunctions.cpp

        scene/cnode.cpp

        plat/coffee_windows/win_core.cpp
        plat/coffee_unix/unix_core.cpp
        plat/argument_parse.cpp
        plat/environment_details.cpp
        plat/plat_file.cpp
        plat/plat_timers.cpp
        plat/cmemory.cpp
        plat/plat_libraries.cpp
        )

set(DL_LIBRARY "")
if(NOT WIN32)
        list(APPEND DL_LIBRARY dl)
endif()

add_library( CoffeeCore ${COFFEE_LINK_OPT} ${CoffeeCore_SRCS} )

#Libraries for X11 and Linux
if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    target_include_directories(CoffeeCore SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()
############################

find_package(SDL2 REQUIRED)
include_directories(
    ${SDL2_INCLUDE_DIR}
    ${GLBINDING_INCLUDE_DIRS}
    )

target_link_libraries ( CoffeeCore
${SDL2_LIBRARY}
${GLBINDING_LIBRARIES}
${LIBUNWIND_LIBRARIES}
${DL_LIBRARY}
)

if(WIN32)
    target_link_libraries ( CoffeeCore
        pathcch
        )
endif()

target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
