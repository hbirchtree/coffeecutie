project( CoffeeCore )
cmake_minimum_required( VERSION 3.1 )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

set ( CMAKE_BUILD_TYPE Debug )

set ( COFFEE_EXT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs )

set ( COFFEE_BUILD_STATIC false )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

add_definitions ( -Wall )
include_directories (
                ${CMAKE_SOURCE_DIR}
                include
                libs
                coffee)

if (COFFEE_BUILD_STATIC)
    set(COFFEE_LINK_OPT STATIC)
else()
    set(COFFEE_LINK_OPT SHARED)
endif()

#Assimp currently needs to be included in the global namespace.
#But not for long.
if(NOT DEFINED ASSIMP_INCLUDE_DIR)
    set (ASSIMP_INCLUDE_DIR ${COFFEE_EXT_LIBRARY_DIR}/assimp/include)
endif()

include_directories (
                    ${ASSIMP_INCLUDE_DIR}
                    )

add_subdirectory(coffee)
add_subdirectory(examples)
add_subdirectory(docs)
