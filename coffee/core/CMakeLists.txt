if(NOT DEFINED GLBINDING_INCLUDE_DIR)
    set (GLBINDING_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/glbinding/source/glbinding/include)
endif()

include_directories (
                    ${GLBINDING_INCLUDE_DIR}
                    )

if(NOT DEFINED GLBINDING_LIBRARY_DIR)
    set (GLBINDING_LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib )
endif()


find_library(GLBINDING_LIBRARY NAMES "glbinding" PATHS ${GLBINDING_LIBRARY_DIR} )

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
        graphics/opengl_glbinding/glbinding.cpp
        graphics/opengl_glbinding/copengl_types.cpp
        graphics/opengl_glbinding/cuniformtypes.cpp

        jobs/ctasksystem.cpp

        unit_tests/data_types.cpp

        input/cinputfunctions.cpp

        scene/cnode.cpp

        plat/coffee_windows/win_core.cpp
        plat/plat_file.cpp
        plat/plat_timers.cpp
        plat/cmemory.cpp
        plat/plat_libraries.cpp
        )

add_library( CoffeeCore ${COFFEE_LINK_OPT} ${CoffeeCore_SRCS} )

find_package(SDL2 REQUIRED)
include_directories(
    ${SDL2_INCLUDE_DIR}
    )

#Libraries for X11 and Linux
if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    target_include_directories(CoffeeCore SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()
############################

target_link_libraries ( CoffeeCore
${SDL2_LIBRARY}
${GLBINDING_LIBRARY}
${LIBUNWIND_LIBRARIES}
${X11_LIBRARIES}
${M_LIB_LIBRARIES}
)

target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
