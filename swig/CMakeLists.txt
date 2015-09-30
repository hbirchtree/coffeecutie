cmake_minimum_required(VERSION 3.1)

set( CMAKE_VERBOSE_MAKEFILE on )

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules")

set ( CoffeeCore_SRCS
        ../main.cpp

        ../coffee/cobject.cpp
        ../coffee/display/cdrendererbase.cpp

        ../coffee_impl/assimp/cassimpimporters.cpp

        ../coffee_impl/context/cdrenderer.cpp
        ../coffee_impl/context/cglbindingrenderer.cpp

        ../coffee_impl/display/csdl2renderer.cpp

        ../coffee_impl/functional/cqueuerunner.cpp

        ../coffee_impl/graphics/cshader.cpp
        ../coffee_impl/graphics/cuniformchunk.cpp
        ../coffee_impl/graphics/cgraphicsdata.cpp

        ../coffee_impl/rendering/cmultidrawgraph.cpp

        ../include/stb/stb_vorbis.c
        )

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/assimp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/glbinding/source/glbinding/include
)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CFILE "${CMAKE_SOURCE_DIR}/coffeeswig.i")
set(CMAKE_SWIG_FLAGS "")

set( COFFEE_LINKING CoffeeLib )

add_library( CoffeeLib SHARED ${CoffeeCore_SRCS} )

find_library(assimp_loc NAMES "assimp" PATHS ${CMAKE_CURRENT_BINARY_DIR} )
find_library(glbinding_loc NAMES "glbinding" PATHS ${CMAKE_CURRENT_BINARY_DIR} )

find_package(SDL2 REQUIRED)
target_include_directories(CoffeeLib SYSTEM PRIVATE "${SDL2_INCLUDE_DIR}")

if(NOT WIN32)
        find_package(Unwind)
        target_include_directories(CoffeeLib SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()

target_link_libraries ( CoffeeLib "${SDL2_LIBRARY}" ${LIBUNWIND_LIBRARIES} ${assimp_loc} ${glbinding_loc} ${X11_LIBRARIES} )

set_source_files_properties(${CFILE} PROPERTIES CPLUSPLUS ON)

if(COFFEE_CSHARP)
#CSharp build
swig_add_module(Coffee csharp ${CFILE} )
swig_link_libraries(Coffee ${COFFEE_LINKING} )

else()
#Python build
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

swig_add_module(Coffee python ${CFILE} )
swig_link_libraries(Coffee ${COFFEE_LINKING} ${PYTHON_LIBRARIES} )

endif()
