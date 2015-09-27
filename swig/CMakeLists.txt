cmake_minimum_required(VERSION 3.1)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs/glbinding/source/glbinding/include)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CFILE "${CMAKE_SOURCE_DIR}/coffeeswig.i")
set(CMAKE_SWIG_FLAGS "")

set( COFFEE_LINKING unwind )

add_library(coffeelib SHARED "../main.cpp" )
target_compile_features(coffeelib PRIVATE cxx_variadic_templates)
target_link_libraries(coffeelib ${COFFEE_LINKING} )

set_source_files_properties(${CFILE} PROPERTIES CPLUSPLUS ON)

if(COFFEE_CSHARP)
#CSharp build
swig_add_module(Coffee csharp ${CFILE} )
swig_link_libraries(Coffee coffeelib ${COFFEE_LINKING} )

else()
#Python build
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

swig_add_module(Coffee python ${CFILE} )
swig_link_libraries(Coffee coffeelib ${COFFEE_LINKING} ${PYTHON_LIBRARIES} )

endif()
