project( CoffeeCore )
cmake_minimum_required( VERSION 3.1 )
set ( CMAKE_BUILD_TYPE Debug )

set ( COFFEE_BUILD_STATIC false )

MACRO(GENERATE_DOCUMENTATION DOXYGEN_CONFIG_FILE)
FIND_PACKAGE(Doxygen)
SET(DOXYFILE_FOUND false)

set (DOXYGEN_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")

IF(EXISTS ${CMAKE_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})
        configure_file(${CMAKE_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        SET(DOXYFILE_FOUND true)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})

IF( DOXYGEN_FOUND )
        IF( DOXYFILE_FOUND )
                # Add target

                add_custom_target(make_docdir ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUT_DIR})
                ADD_CUSTOM_TARGET(doc ALL ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                        WORKING_DIRECTORY ${DOXYGEN_OUT_DIR})

        ELSE( DOXYFILE_FOUND )
                MESSAGE( STATUS "Doxygen configuration file not found - Documentation will not be generated" )
        ENDIF( DOXYFILE_FOUND )
ELSE(DOXYGEN_FOUND)
        MESSAGE(STATUS "Doxygen not found - Documentation will not be generated")
ENDIF(DOXYGEN_FOUND)
ENDMACRO(GENERATE_DOCUMENTATION)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

add_definitions ( -Wall )
include_directories (
                ${CMAKE_SOURCE_DIR}
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_CURRENT_BINARY_DIR}
                libs
                libs/glbinding/source/glbinding/include
                libs/assimp/include
                coffee)

set ( CoffeeCore_SRCS

        coffee/core/coffee.cpp
        coffee/core/base/counter.cpp
        coffee/core/base/cdisplay.cpp
        coffee/core/base/cfiles.cpp
        coffee/core/base/cobject.cpp
        coffee/core/base/cregex.cpp

        coffee/core/base/cdrendererbase.cpp

        coffee/core/context/cglbindingrenderer.cpp

        coffee/core/display/csdl2renderer.cpp
        coffee/core/display/coffeesdl2/sdl2eventhandlers.cpp
        coffee/core/display/coffeesdl2/sdl2helpers.cpp

        coffee/core/functional/cqueuerunner.cpp

        coffee/core/graphics/cbuffer.cpp
        coffee/core/graphics/cshader.cpp
        coffee/core/graphics/cgraphicsdata.cpp
        coffee/core/graphics/cvertexarrayobject.cpp
        coffee/core/graphics/ctexture.cpp
        coffee/core/graphics/ctexture_dxtc.cpp
        coffee/core/graphics/cgraphics_quirks.cpp
        coffee/core/unit_tests/data_types.cpp

        coffee/core/input/cinputfunctions.cpp

        #Third-party sources
        libs/cppformat/format.cc
        )

set ( CoffeeTest_SRCS
        tests/ctest/main.cpp
        tests/ctest/rendering/cmultidrawgraph.cpp
        tests/ctest/base_case.cpp
        tests/ctest/cdrenderer.cpp
        )

set ( CoffeeTestHud_SRCS
        tests/ctest-hud/main.cpp
        )

set ( CAssimp_SRCS
        coffee/assimp/cassimpimporters.cpp
        coffee/assimp/assimpfun.cpp
        )

set ( CAudio_SRCS
        libs/stb/stb_vorbis.c
        coffee/audio/caudio.cpp
        coffee/audio/openal/copenal.cpp
        )

set ( CImage_SRCS
        coffee/image/cimage.cpp
        )

set ( CBlamVolta_SRCS
        coffee/blam/volta/cblam_bitm.cpp
        coffee/blam/volta/cblam_map.cpp
        coffee/blam/volta/cblam_mod2.cpp
        coffee/blam/volta/cblam_scenario_loader.cpp
        )
set ( CBlamDimeter_SRCS
        coffee/blam/dimeter/cblam_map.cpp
        )
set ( CBlamElegy_SRCS
        coffee/blam/elegy/cblam_map.cpp
        )

set ( CBlamTesting_SRCS
        tests/cblam-testing/main.cpp
        )

#Locate Assimp and glbinding libraries
find_library(assimp_loc NAMES "assimp" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )
find_library(glbinding_loc NAMES "glbinding" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )

message("Assimp located at: ${assimp_loc}")
message("GLbinding located at: ${glbinding_loc}")
######################################

#Set up build targets
if (COFFEE_BUILD_STATIC)
    add_library( CBlam STATIC ${CBlamVolta_SRCS} ${CBlamDimeter_SRCS} ${CBlamElegy_SRCS})
    add_library( CoffeeCore STATIC ${CoffeeCore_SRCS} )
    add_library( CAudio STATIC ${CAudio_SRCS})
    add_library( CAssimp STATIC ${CAssimp_SRCS})
    add_library( CImage STATIC ${CImage_SRCS})
    add_library ( CoffeeTest STATIC ${CoffeeTest_SRCS} )
    add_executable ( CBlamTesting ${CBlamTesting_SRCS} )
    add_executable ( CoffeeTest ${CoffeeTest_SRCS} )
    add_executable ( CoffeeTestHud ${CoffeeTestHud_SRCS} )
else()
    add_library( CBlam SHARED ${CBlamVolta_SRCS} ${CBlamDimeter_SRCS} ${CBlamElegy_SRCS})
    add_library( CoffeeCore SHARED ${CoffeeCore_SRCS} )
    add_library( CAudio SHARED ${CAudio_SRCS})
    add_library( CAssimp SHARED ${CAssimp_SRCS})
    add_library( CImage SHARED ${CImage_SRCS})
    add_library ( CoffeeTestLib SHARED ${CoffeeTest_SRCS} ) #For SWIG
    add_executable ( CBlamTesting ${CBlamTesting_SRCS} )
    add_executable ( CoffeeTest ${CoffeeTest_SRCS} )
    add_executable ( CoffeeTestHud ${CoffeeTestHud_SRCS} )
endif()
#####################

#Set up necessary linking options
find_package(OpenAL REQUIRED)
target_include_directories(CAudio SYSTEM PRIVATE "${OPENAL_INCLUDE_DIR}")
#target_include_directories(CoffeeCore SYSTEM PRIVATE "${OPENAL_INCLUDE_DIR}")

find_package(SDL2 REQUIRED)
target_include_directories(CoffeeCore SYSTEM PRIVATE "${SDL2_INCLUDE_DIR}")

#################################

#Libraries for X11 and Linux
if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    target_include_directories(CoffeeCore SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()
############################

#Configure linking options
target_link_libraries ( CoffeeCore
${SDL2_LIBRARY}
${glbinding_loc}
${LIBUNWIND_LIBRARIES}
${X11_LIBRARIES}
${M_LIB_LIBRARIES}
)
target_link_libraries ( CAssimp
${assimp_loc}
)
target_link_libraries ( CAudio
${OPENAL_LIBRARY}
)
target_link_libraries ( CoffeeTestLib
CoffeeCore
${glbinding_loc}
CBlam
CAudio
CImage
CAssimp
)
target_link_libraries ( CoffeeTest
CoffeeCore
CBlam
CAudio
CImage
CAssimp
)
target_link_libraries ( CoffeeTestHud
CoffeeCore
CImage
)
target_link_libraries ( CBlamTesting
CoffeeCore
CImage
CBlam
)
##########################

#Configure PIC and C++11
target_compile_features(CBlamTesting PRIVATE cxx_variadic_templates)

target_compile_features(CBlam PRIVATE cxx_variadic_templates)
target_compile_features(CAudio PRIVATE cxx_variadic_templates)
target_compile_features(CAssimp PRIVATE cxx_variadic_templates)
target_compile_features(CImage PRIVATE cxx_variadic_templates)

target_compile_features(CoffeeTestLib PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTestHud PRIVATE cxx_variadic_templates)

target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
########################

#Documentation with Doxygen
GENERATE_DOCUMENTATION("docs/coffee.dox.in")
