project( CoffeeCore C CXX )
cmake_minimum_required( VERSION 3.1 )

option ( COFFEE_BUILD_SWIG_BINDING "Build SWIG C# binding" OFF )
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set ( CMAKE_BUILD_TYPE Debug )
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

add_definitions ( -Wall )
include_directories (
                ${CMAKE_SOURCE_DIR}
                ${CMAKE_BINARY_DIR}/libs/include
                include
                libs
                coffee)

link_directories(
    ${CMAKE_BINARY_DIR}/libs/
    )

if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

add_custom_target(DummyPlug)

if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    include_directories("${LIBUNWIND_INCLUDE_DIR}")
endif()

add_subdirectory(coffee)
add_subdirectory(examples)
add_subdirectory(docs)

if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(swig)
endif()

#add_subdirectory(libs)
