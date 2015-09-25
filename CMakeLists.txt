project( CoffeeSharp )
cmake_minimum_required( VERSION 2.8 )
set ( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	include
	libs/glbinding/source/glbinding/include
	libs/assimp/include
	libs/glfw/include
	libs/bullet/src)
set ( CoffeeCore_HDRS
	coffee_impl/display/cglfwrenderer.h
	coffee_impl/display/coffeeglfw/cglfwrenderer_eventhandlers.h
	coffee_impl/display/coffeeglfw/cglfwnativefuncs.h
	coffee_impl/display/csdl2renderer.h
	coffee_impl/display/coffeesdl2/sdl2datatypes.h
	tests/CubeScape.h
	tests/glutils.h
	tests/RawFile.h
	coffee.h
	coffee/display/cdrendererbase.h
	coffee/cobject.h
	coffee/cdebug.h
	coffee/cfunctional.h
	coffee/cdisplay.h
	coffee/cinput.h
	coffee_impl/context/cdrenderer.h
	coffee_impl/functional/cqueuerunner.h
	coffee_impl/graphics/cgraphicswrappers.h
	coffee_impl/graphics/cshader.h
	coffeesharp.h
	coffee/cfiles.h
	coffee/cregex.h
	coffee_impl/assimp/cassimpimporters.h
	coffee_impl/storage/cdatastore.h
	coffee_impl/assimp/assimpfun.h
	coffee_impl/assimp/cassimptypes.h
	coffee_impl/assimp/cassimploader.h
	coffee_impl/graphics/cgraphicsdata.h
	unit_tests/data_types.h
	coffee_impl/graphics/cbuffer.h
	coffee_impl/graphics/glbinding.h
	coffee_impl/graphics/cvertexarrayobject.h
	coffee_impl/graphics/cuniformtypes.h
	coffee_impl/graphics/cframebuffer.h
	coffee_impl/graphics/ctexture.h
	coffee_impl/context/cglbindingrenderer.h
	plat/linux_wm.h
	coffee_impl/graphics/ctransformcomputer.h
	coffee_impl/rendering/cmultidrawgraph.h
	coffee_impl/graphics/cuniformchunk.h
	coffee_impl/display/coffeesdl2/sdl2helpers.h
	coffee_impl/display/coffeesdl2/sdl2eventhandlers.h
	coffee_impl/input/cinput_eventhandling.h
	coffee/cinput_keymap.h
	coffee_impl/memory/cgame_dataset.h
	coffee_impl/sample/base_case.h
	)

set ( CoffeeCore_SRCS
	coffee_impl/display/csdl2renderer.cpp
	main.cpp
	tests/CubeScape.cpp
	tests/glutils.cpp
	tests/RawFile.cpp
	coffee/display/cdrendererbase.cpp
	coffee/cobject.cpp
	coffee_impl/context/cdrenderer.cpp
	coffee_impl/functional/cqueuerunner.cpp
	coffee_impl/graphics/cshader.cpp
	coffee_impl/assimp/cassimpimporters.cpp
	coffee_impl/storage/cdatastore.cpp
	coffee_impl/graphics/cgraphicsdata.cpp
	coffee_impl/context/cglbindingrenderer.cpp
	coffee_impl/rendering/cmultidrawgraph.cpp
	coffee_impl/graphics/cuniformchunk.cpp
	)

#We want to compile GLbinding and Assimp automatically with CoffeeCore

include(ExternalProject)

#ExternalProject_Add(
#	glbinding_target
#	SOURCE_DIR ../libs/glbinding
#	PREFIX ../libs/glbinding
#	BINARY_DIR compile/glbinding
#	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=lib/glbinding
#)
#ExternalProject_Add(
#	assimp_target
#	SOURCE_DIR ../libs/assimp
#	PREFIX ../libs/assimp
#	BINARY_DIR compile/assimp
#	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=lib/assimp
#)

find_library(assimp_loc NAMES "assimp" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )
find_library(glbinding_loc NAMES "glbinding" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )

message("Assimp located at: ${assimp_loc}")
message("GLbinding located at: ${glbinding_loc}")

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#PKG_SEARCH_MODULE(UNWIND REQUIRED unwind)

add_executable ( CoffeeCore ${CoffeeCore_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )

#add_dependencies(CoffeeCore glbinding_target assimp_target)

target_link_libraries ( CoffeeCore ${SDL2_LIBRARIES} unwind ${assimp_loc} ${glbinding_loc} )
set_property(TARGET CoffeeCore PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
