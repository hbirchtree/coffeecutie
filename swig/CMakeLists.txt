cmake_minimum_required(VERSION 3.1)

add_library(coffeeswag "../main.cpp" )
target_link_libraries(coffeeswag)
target_compile_features(coffeeswag PRIVATE cxx_variadic_templates)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs/glbinding/source/glbinding/include)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CFILE "${CMAKE_SOURCE_DIR}/coffeeswig.i")
set(CMAKE_SWIG_FLAGS "")

set_source_files_properties(${CFILE} PROPERTIES CPLUSPLUS ON)
#set_source_files_properties(coffeeswig.i PROPERTIES SWIG_FLAGS "-includeall")
swig_add_module(coffeeswig csharp ${CFILE} coffeeswig_wrap.cxx)
swig_link_libraries(coffeeswig coffeeswag )
