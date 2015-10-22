project( CoffeeCore )
cmake_minimum_required( VERSION 3.1 )
set ( CMAKE_BUILD_TYPE Release )

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
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        include
        libs/glbinding/source/glbinding/include
        libs/assimp/include)

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

        coffee.cpp
        coffee/cobject.cpp
        coffee/cregex.cpp
        coffee/cfiles.cpp

        coffee/display/cdrendererbase.cpp

        coffee_impl/assimp/cassimpimporters.cpp
        coffee_impl/assimp/assimpfun.cpp

        coffee_impl/context/cdrenderer.cpp
        coffee_impl/context/cglbindingrenderer.cpp

        coffee_impl/display/csdl2renderer.cpp
        coffee_impl/display/coffeesdl2/sdl2eventhandlers.cpp
        coffee_impl/display/coffeesdl2/sdl2helpers.cpp

        coffee_impl/functional/cqueuerunner.cpp

        coffee_impl/graphics/cbuffer.cpp
        coffee_impl/graphics/cshader.cpp
        coffee_impl/graphics/cgraphicsdata.cpp
        coffee_impl/graphics/cvertexarrayobject.cpp
        coffee_impl/graphics/ctexture.cpp
        coffee_impl/graphics/ctexture_dxtc.cpp
        coffee_impl/graphics/cgraphics_quirks.cpp

        coffee_impl/rendering/cmultidrawgraph.cpp

        coffee_impl/sample/base_case.cpp

#Third-party sources

        include/cppformat/format.cc
        )

set ( CoffeeTest_SRCS
        main.cpp
        )

set ( CBlam_SRCS
        datasources/blam/cblam_bitm.cpp
        datasources/blam/cblam_map.cpp
        )

set ( CAudio_SRCS
        include/stb/stb_vorbis.c
        coffee_impl/audio/caudio.cpp
        coffee_impl/audio/openal/copenal.cpp
        )

set ( CImage_SRCS
        coffee_impl/image/cimage.cpp
        )


#Locate Assimp and glbinding libraries
find_library(assimp_loc NAMES "assimp" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )
find_library(glbinding_loc NAMES "glbinding" PATHS ${CMAKE_CURRENT_BINARY_DIR}/lib )

message("Assimp located at: ${assimp_loc}")
message("GLbinding located at: ${glbinding_loc}")
######################################

#Set up build targets
add_library( CBlam SHARED ${CBlam_SRCS})
add_library( CoffeeLib SHARED ${CoffeeCore_SRCS} )
add_library( CAudio SHARED ${CAudio_SRCS})
add_library( CImage SHARED ${CImage_SRCS})
add_executable ( CoffeeTest ${CoffeeTest_SRCS} )
#####################

#Set up necessary linking options
find_package(OpenAL REQUIRED)
target_include_directories(CAudio SYSTEM PRIVATE "${OPENAL_INCLUDE_DIR}")
#target_include_directories(CoffeeLib SYSTEM PRIVATE "${OPENAL_INCLUDE_DIR}")

find_package(SDL2 REQUIRED)
target_include_directories(CoffeeLib SYSTEM PRIVATE "${SDL2_INCLUDE_DIR}")

#################################

#Libraries for X11 and Linux
if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    target_include_directories(CoffeeLib SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()
############################

#Configure linking options
target_link_libraries ( CoffeeLib
${SDL2_LIBRARY}
${LIBUNWIND_LIBRARIES}
${assimp_loc}
${glbinding_loc}
${X11_LIBRARIES}
${M_LIB_LIBRARIES}
${OPENAL_LIBRARY} )
target_link_libraries ( CoffeeTest
${SDL2_LIBRARY}
${LIBUNWIND_LIBRARIES}
${assimp_loc}
${glbinding_loc}
${X11_LIBRARIES}
${OPENAL_LIBRARY}
CoffeeLib
CBlam
CAudio
CImage )
##########################

#Configure PIC and C++11
set_property(TARGET CBlam PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CBlam PRIVATE cxx_variadic_templates)

set_property(TARGET CAudio PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CAudio PRIVATE cxx_variadic_templates)

set_property(TARGET CImage PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CImage PRIVATE cxx_variadic_templates)

set_property(TARGET CoffeeTest PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CoffeeTest PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeTest PRIVATE cxx_constexpr)

set_property(TARGET CoffeeLib PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(CoffeeLib PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeLib PRIVATE cxx_constexpr)
########################

#Documentation with Doxygen
GENERATE_DOCUMENTATION("docs/coffee.dox")
