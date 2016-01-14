cmake_minimum_required(VERSION 3.1)

set( CMAKE_VERBOSE_MAKEFILE on )

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(
        ${CMAKE_SOURCE_DIR}/../
        
        ${CMAKE_SOURCE_DIR}/../coffee/assimp/include
        ${CMAKE_SOURCE_DIR}/../coffee/audio/include
        ${CMAKE_SOURCE_DIR}/../coffee/blam/include
        ${CMAKE_SOURCE_DIR}/../coffee/core/include
        ${CMAKE_SOURCE_DIR}/../coffee/core/base
        ${CMAKE_SOURCE_DIR}/../coffee/core/plat
        ${CMAKE_SOURCE_DIR}/../coffee/graphics_apis/include
        ${CMAKE_SOURCE_DIR}/../coffee/graphics_apis/gleam/include
        ${CMAKE_SOURCE_DIR}/../coffee/image/include
        ${CMAKE_SOURCE_DIR}/../coffee/media/include
        ${CMAKE_SOURCE_DIR}/../coffee/openvr/include
        ${CMAKE_SOURCE_DIR}/../coffee/sdl2/include
        
        ${CMAKE_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CFILE "${CMAKE_CURRENT_SOURCE_DIR}/coffeeswig.i")
set(CMAKE_SWIG_FLAGS "")

link_directories( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${COFFEE_LIB_DIR})

set(COFFEE_LINKING CoffeeCore CAudio CBlam CoffeeOpenGL CoffeeTestLib)

set_source_files_properties(${CFILE} PROPERTIES CPLUSPLUS ON)
set_property(SOURCE ${CFILE} PROPERTY SWIG_FLAGS "-ignoremissing")

if(COFFEE_PYTHON)
        #Python build
        find_package(PythonLibs)
        include_directories(${PYTHON_INCLUDE_PATH})

        swig_add_module(Coffee python ${CFILE} )
        swig_link_libraries(Coffee ${COFFEE_LINKING} ${PYTHON_LIBRARIES} )
else()
        #CSharp build
        swig_add_module(Coffee csharp ${CFILE} )
        swig_link_libraries(Coffee ${COFFEE_LINKING} )
endif()
