project( CoffeeCore C CXX )
cmake_minimum_required( VERSION 3.1 )

option ( COFFEE_BUILD_SWIG_BINDING "Build SWIG C# binding" OFF )
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set ( CMAKE_BUILD_TYPE Debug )
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

if (NOT WIN32)
    add_definitions ( -Wall )
else()
    add_definitions ( -W4 )
endif()

include_directories (
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/libs/include
        include
        coffee
    )

include_directories ( SYSTEM
        libs
    )

if(CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

link_directories(
    ${CMAKE_BINARY_DIR}/libs/lib/
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

#Include directories; I'm putting them here temporarily
set (
    COFFEE_ASSIMP_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/assimp/include
    )
set (
    COFFEE_AUDIO_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/audio/include
    )
set (
    COFFEE_BLAM_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/blam/include
    )
set (
    COFFEE_CORE_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/core/include
    )
set (
    COFFEE_GRAPHICS_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/include
    )
set (
    COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/graphics_apis/opengl/include
    )
set (
    COFFEE_IMAGE_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/coffee/image/include
    )

add_subdirectory(coffee)

add_subdirectory(examples)
add_subdirectory(docs)

if(COFFEE_BUILD_SWIG_BINDING)
    add_subdirectory(swig)
endif()

#add_subdirectory(libs)

#PCL extension

set (
    COFFEE_EXT_PCL_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/ext/pcl-shim/include
    )

#add_subdirectory(ext)
