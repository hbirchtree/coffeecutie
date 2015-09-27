project( CoffeeCore )
cmake_minimum_required( VERSION 3.1 )
set ( CMAKE_BUILD_TYPE Debug )

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

add_definitions ( -Wall )
include_directories (
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        include
        libs/glbinding/source/glbinding/include
        libs/assimp/include
        libs/glfw/include
        libs/bullet/src)
set ( CoffeeCore_HDRS
        cppsharp/coffeesharp.h

        coffee.h
        coffee/display/cdrendererbase.h
        coffee/cobject.h
        coffee/cdebug.h
        coffee/cfunctional.h
        coffee/cdisplay.h
        coffee/cinput.h
        coffee/cfiles.h
        coffee/cregex.h
        coffee/cinput_keymap.h

        coffee_impl/assimp/cassimpimporters.h
        coffee_impl/assimp/assimpfun.h
        coffee_impl/assimp/cassimptypes.h
        coffee_impl/assimp/cassimploader.h

        coffee_impl/context/cdrenderer.h
        coffee_impl/context/cglbindingrenderer.h

        coffee_impl/display/csdl2renderer.h
        coffee_impl/display/coffeesdl2/sdl2datatypes.h
        coffee_impl/display/coffeesdl2/sdl2helpers.h
        coffee_impl/display/coffeesdl2/sdl2eventhandlers.h

        coffee_impl/functional/cqueuerunner.h

        coffee_impl/graphics/cgraphicswrappers.h
        coffee_impl/graphics/cshader.h
        coffee_impl/graphics/cgraphicsdata.h
        coffee_impl/graphics/cbuffer.h
        coffee_impl/graphics/glbinding.h
        coffee_impl/graphics/cvertexarrayobject.h
        coffee_impl/graphics/cuniformtypes.h
        coffee_impl/graphics/cframebuffer.h
        coffee_impl/graphics/ctexture.h
        coffee_impl/graphics/cuniformchunk.h
        coffee_impl/graphics/ctransformcomputer.h

        coffee_impl/input/cinput_eventhandling.h

        coffee_impl/memory/cgame_dataset.h

        coffee_impl/rendering/cmultidrawgraph.h

        coffee_impl/sample/base_case.h

        plat/plat_wm.h
        plat/linux/linux_wm.h
        plat/plat_core.h
        plat/unix/unix_core.h
        plat/windows/win_wm.h
        plat/windows/win_core.h

        plat/argument_parse.h
        plat/platform_detect.h

        unit_tests/data_types.h
        )

set ( CoffeeCore_SRCS
        main.cpp

        coffee/cobject.cpp
        coffee/display/cdrendererbase.cpp

        coffee_impl/assimp/cassimpimporters.cpp

        coffee_impl/context/cdrenderer.cpp
        coffee_impl/context/cglbindingrenderer.cpp

        coffee_impl/display/csdl2renderer.cpp

        coffee_impl/functional/cqueuerunner.cpp

        coffee_impl/graphics/cshader.cpp
        coffee_impl/graphics/cuniformchunk.cpp
        coffee_impl/graphics/cgraphicsdata.cpp

        coffee_impl/rendering/cmultidrawgraph.cpp
        )

if(NOT WIN32)
    find_package(X11)
endif()

include(ExternalProject)

find_library(assimp_loc NAMES "assimp" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )
find_library(glbinding_loc NAMES "glbinding" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )

message("Assimp located at: ${assimp_loc}")
message("GLbinding located at: ${glbinding_loc}")

add_library( CoffeeLib SHARED ${CoffeeCore_SRCS} )
add_executable ( CoffeeCore ${CoffeeCore_SRCS} )

find_package(SDL2 REQUIRED)
target_include_directories(CoffeeCore SYSTEM PRIVATE "${SDL2_INCLUDE_DIR}")
target_include_directories(CoffeeLib SYSTEM PRIVATE "${SDL2_INCLUDE_DIR}")

if(NOT WIN32)
        find_package(Unwind)
        target_include_directories(CoffeeCore SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()

target_link_libraries ( CoffeeCore "${SDL2_LIBRARY}" ${LIBUNWIND_LIBRARIES} ${assimp_loc} ${glbinding_loc} ${X11_LIBRARIES} )
target_link_libraries ( CoffeeLib "${SDL2_LIBRARY}" ${LIBUNWIND_LIBRARIES} ${assimp_loc} ${glbinding_loc} ${X11_LIBRARIES} )
set_property(TARGET CoffeeCore PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)

set_property(TARGET CoffeeLib PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CoffeeLib PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeLib PRIVATE cxx_constexpr)
