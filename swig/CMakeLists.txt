cmake_minimum_required(VERSION 3.1)

set( CMAKE_VERBOSE_MAKEFILE on )

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories( ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../include )

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CFILE "${CMAKE_SOURCE_DIR}/coffeeswig.i")
set(CMAKE_SWIG_FLAGS "")

link_directories( ${CMAKE_BINARY_DIR})

set(COFFEE_LINKING CoffeeCore CAudio CBlam CImage)

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
