cmake_minimum_required(VERSION 3.1)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(CFILE "${CMAKE_SOURCE_DIR}/coffeeswig.i")
set(CMAKE_SWIG_FLAGS "")

set_source_files_properties(${CFILE} PROPERTIES CPLUSPLUS ON)
#set_source_files_properties(coffeeswig.i PROPERTIES SWIG_FLAGS "-includeall")
swig_add_module(coffeeswig python ${CFILE} coffeeswig_wrap.cxx)
swig_link_libraries(coffeeswig ${PYTHON_LIBRARIES})

swig_compile_features(coffeeswig PRIVATE cxx_variadic_templates)
