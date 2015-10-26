project( CoffeeCore )
cmake_minimum_required( VERSION 3.1 )
set ( CMAKE_BUILD_TYPE Debug )

set ( COFFEE_BUILD_STATIC false )

MACRO(GENERATE_DOCUMENTATION DOXYGEN_CONFIG_FILE)
FIND_PACKAGE(Doxygen)
SET(DOXYFILE_FOUND false)
IF(EXISTS ${PROJECT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})
     SET(DOXYFILE_FOUND true)
ENDIF(EXISTS ${PROJECT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})

IF( DOXYGEN_FOUND )
     IF( DOXYFILE_FOUND )
        # Add target
         ADD_CUSTOM_TARGET( doc ALL ${DOXYGEN_EXECUTABLE}
"${PROJECT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE}" )

     ELSE( DOXYFILE_FOUND )
        MESSAGE( STATUS "Doxygen configuration file not found - Documentation
will not be generated" )
     ENDIF( DOXYFILE_FOUND )
ELSE(DOXYGEN_FOUND)
     MESSAGE(STATUS "Doxygen not found - Documentation will not be generated")
ENDIF(DOXYGEN_FOUND)
ENDMACRO(GENERATE_DOCUMENTATION)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

add_definitions ( -Wall )
include_directories (
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        libs
        libs/glbinding/source/glbinding/include
        libs/assimp/include
        coffee)

set ( CoffeeCore_HDRS
coffee/cfiles.h
coffee/cinput_keymap.h
coffee/cinput.h
coffee/cthreading.h
coffee/cmath.h
coffee/display/cdrendererbase.h
coffee/cfunctional.h
coffee/cdebug.h
coffee/cobject.h
coffee/cdisplay.h
coffee/cregex.h
coffee_impl/functional/cqueuerunner.h
coffee_impl/display/coffeeglfw/cglfwnativefuncs.h
coffee_impl/display/coffeeglfw/cglfwrenderer_eventhandlers.h
coffee_impl/display/csdl2renderer.h
coffee_impl/display/coffeesdl2/sdl2helpers.h
coffee_impl/display/coffeesdl2/sdl2datatypes.h
coffee_impl/display/coffeesdl2/sdl2eventhandlers.h
coffee_impl/display/cglfwrenderer.h
coffee_impl/audio/caudio.h
coffee_impl/audio/openal/copenal.h
coffee_impl/rendering/cmultidrawgraph.h
coffee_impl/memory/cgame_dataset.h
coffee_impl/memory/cgame_function_binds.h
coffee_impl/image/cimage.h
coffee_impl/input/cinput_eventhandling.h
coffee_impl/sample/base_case.h
coffee_impl/graphics/glbinding.h
coffee_impl/graphics/cvertexarrayobject.h
coffee_impl/graphics/ctexture.h
coffee_impl/graphics/cgraphicsdata.h
coffee_impl/graphics/ctexture_dxtc.h
coffee_impl/graphics/cuniformtypes.h
coffee_impl/graphics/cshader.h
coffee_impl/graphics/cgraphicswrappers.h
coffee_impl/graphics/cbuffer.h
coffee_impl/graphics/cframebuffer.h
coffee_impl/graphics/ctransformcomputer.h
coffee_impl/graphics/cgraphics_quirks.h
coffee_impl/assimp/cassimpimporters.h
coffee_impl/assimp/cassimptypes.h
coffee_impl/assimp/assimpfun.h
coffee_impl/context/cdrenderer.h
coffee_impl/context/cglbindingrenderer.h

datasources/blam/cblam_map.h
datasources/blam/cblam.h
datasources/blam/cblam_bitm.h

unit_tests/data_types.h
plat/platform_detect.h
plat/argument_parse.h
plat/coffee_windows/win_core.h
plat/coffee_windows/win_wm.h
plat/coffee_linux/linux_wm.h
plat/environment_details.h
plat/plat_wm.h
plat/coffee_unix/unix_core.h
plat/plat_core.h
        )

set ( CoffeeCore_SRCS

        coffee/core/coffee.cpp
        coffee/core/base/cobject.cpp
        coffee/core/base/cregex.cpp
        coffee/core/base/cfiles.cpp

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

#Third-party sources

        libs/cppformat/format.cc
        )

set ( CoffeeTest_SRCS
        tests/ctest/main.cpp
        tests/ctest/rendering/cmultidrawgraph.cpp
        tests/ctest/base_case.cpp
        tests/ctest/cdrenderer.cpp
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
        )
set ( CBlamDimeter_SRCS
        coffee/blam/dimeter/cblam_map.cpp
        )
set ( CBlamElegy_SRCS
        coffee/blam/elegy/cblam_map.cpp
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
    add_executable ( CoffeeTest ${CoffeeTest_SRCS} )
else()
    add_library( CBlam SHARED ${CBlamVolta_SRCS} ${CBlamDimeter_SRCS} ${CBlamElegy_SRCS})
    add_library( CoffeeCore SHARED ${CoffeeCore_SRCS} )
    add_library( CAudio SHARED ${CAudio_SRCS})
    add_library( CAssimp SHARED ${CAssimp_SRCS})
    add_library( CImage SHARED ${CImage_SRCS})
    add_executable ( CoffeeTest ${CoffeeTest_SRCS} )
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
target_link_libraries ( CAssimp
${assimp_loc}
)
target_link_libraries ( CAudio
${OPENAL_LIBRARY}
)
target_link_libraries ( CoffeeCore
${SDL2_LIBRARY}
${LIBUNWIND_LIBRARIES}
${X11_LIBRARIES}
${M_LIB_LIBRARIES}
)
target_link_libraries ( CoffeeTest
CoffeeCore
${glbinding_loc}
CBlam
CAudio
CImage
CAssimp
)
##########################

#Configure PIC and C++11
target_compile_features(CBlam PRIVATE cxx_variadic_templates)
target_compile_features(CAudio PRIVATE cxx_variadic_templates)
target_compile_features(CAssimp PRIVATE cxx_variadic_templates)
target_compile_features(CImage PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_constexpr)
target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
########################

#Documentation with Doxygen
GENERATE_DOCUMENTATION("docs/coffee.dox")
