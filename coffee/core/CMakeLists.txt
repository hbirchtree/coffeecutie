if(NOT DEFINED GLBINDING_INCLUDE_DIR)
    set (GLBINDING_INCLUDE_DIR libs/glbinding/source/glbinding/include)
endif()

if(NOT DEFINED GLBINDING_LIBRARY_DIR)
    set (GLBINDING_LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib )
endif()

include_directories (
                    ${GLBINDING_INCLUDE_DIR}
                    )

find_library(GLBINDING_LIBRARY NAMES "glbinding" PATHS ${GLBINDING_LIBRARY_DIR} )

message("GLBinding location: ${GLBINDING_LIBRARY}")

set ( CoffeeCore_SRCS

        coffee/core/coffee.cpp
        coffee/core/base/counter.cpp
        coffee/core/base/cdebug.cpp
        coffee/core/base/cdisplay.cpp
        coffee/core/base/cfiles.cpp
        coffee/core/base/cobject.cpp
        coffee/core/base/cregex.cpp

        coffee/core/base/cdrendererbase.cpp

        coffee/core/context/cglbindingrenderer.cpp

        coffee/core/display/csdl2renderer.cpp
        coffee/core/display/coffeesdl2/sdl2eventhandlers.cpp
        coffee/core/display/coffeesdl2/sdl2helpers.cpp

        coffee/core/functional/cqueuerunner.cpp

        coffee/core/graphics/cgraphicsdata.cpp
        coffee/core/graphics/cgraphics_quirks.cpp

        coffee/core/graphics/opengl_glbinding/cbuffer.cpp
        coffee/core/graphics/opengl_glbinding/cshader.cpp
        coffee/core/graphics/opengl_glbinding/cframebuffer.cpp
        coffee/core/graphics/opengl_glbinding/cgraphicswrappers.cpp
        coffee/core/graphics/opengl_glbinding/cvertexarrayobject.cpp
        coffee/core/graphics/opengl_glbinding/ctexture.cpp
        coffee/core/graphics/opengl_glbinding/ctexture_dxtc.cpp
        coffee/core/graphics/opengl_glbinding/glbinding.cpp
        coffee/core/graphics/opengl_glbinding/copengl_types.cpp
        coffee/core/graphics/opengl_glbinding/cuniformtypes.cpp

        coffee/core/jobs/ctasksystem.cpp

        coffee/core/unit_tests/data_types.cpp

        coffee/core/input/cinputfunctions.cpp

        coffee/scene/cnode.cpp

        coffee/core/plat/coffee_windows/win_core.cpp
        coffee/core/plat/plat_file.cpp
        coffee/core/plat/plat_timers.cpp
        coffee/core/plat/cmemory.cpp
        )

add_library( CoffeeCore ${COFFEE_LINK_OPT} ${CoffeeCore_SRCS} )

find_package(SDL2 REQUIRED)
target_include_directories(CoffeeCore SYSTEM PRIVATE "${SDL2_INCLUDE_DIR}")

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
CImage
)

target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
