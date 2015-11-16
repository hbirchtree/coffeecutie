project( CoffeeCore )
cmake_minimum_required( VERSION 3.1 )

macro(INCLUDE_SUBPROJECT SUBPROJ_DIR)
    include (${SUBPROJ_DIR}/CMakeLists.txt)
endmacro()

set ( CMAKE_BUILD_TYPE Debug )

set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

set ( COFFEE_BUILD_STATIC true )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

add_definitions ( -Wall )
include_directories (
                ${CMAKE_SOURCE_DIR}
                ${CMAKE_SOURCE_DIR}/include
                libs
                coffee)

if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

include_subproject(coffee)
include_subproject(examples)
include_subproject(docs)
