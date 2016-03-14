cmake_minimum_required(VERSION 3.1)

macro(CONFIG_SWIG_BUILD SFILE)
    # Explicit C++ build
    set_source_files_properties(${SFILE} PROPERTIES CPLUSPLUS ON)
    # Ignore missing headers
    set_property(SOURCE ${SFILE} PROPERTY SWIG_FLAGS "-importall" )
endmacro()

# SWIG import
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories (
    ${CMAKE_SOURCE_DIR}/include

    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Define C++11 flag
set(CMAKE_CXX_FLAGS "-std=c++11")

# We have interfaces in this directory
set(INTERFACE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/interfaces")

set(CFILE_DLANG "${INTERFACE_DIR}/coffeeswig_dlang.i")
set(CFILE_PYTHON "${INTERFACE_DIR}/coffeeswig_python.i")
set(CFILE_SHARP "${INTERFACE_DIR}/coffeeswig_sharp.i")

set(CMAKE_SWIG_FLAGS "")

link_directories( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${COFFEE_LIB_DIR})

set ( CoffeeSwigShim_SRCS
    coffeeswig.cpp
    )

coffee_add_library(CoffeeSwigShim ${CoffeeSwigShim_SRCS} )
target_link_libraries ( CoffeeSwigShim
    CoffeeGLeam
    )

# Link with some libraries
set(COFFEE_LINKING CoffeeOpenAL CoffeeBlam CoffeeSwigShim )

add_subdirectory(csharp)
add_subdirectory(dlang)
add_subdirectory(python)