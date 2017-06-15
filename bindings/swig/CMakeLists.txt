cmake_minimum_required(VERSION 3.1)

macro(CONFIG_SWIG_BUILD SFILE ID)
    # Explicit C++ build
    set_source_files_properties(${SFILE} PROPERTIES CPLUSPLUS ON)
    # Ignore missing headers
    if("${ID}" STREQUAL "python3")
        set_property(SOURCE ${SFILE} PROPERTY SWIG_FLAGS -py3 -importall )
    else()
        set_property(SOURCE ${SFILE} PROPERTY SWIG_FLAGS "-importall" )
    endif()

    set_property ( SOURCE ${CFILE}
        PROPERTY CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11"
        )
endmacro()

macro(SWIG_LANGUAGE_BINDING TARGET BIND CFILE OUTPUT_EXTENSION OUTPUT_DEST )
    CONFIG_SWIG_BUILD(${CFILE} ${OUTPUT_DEST})

    swig_add_module(${TARGET} ${BIND} ${CFILE} )

    if("${BIND}" STREQUAL "python")
        # Python build
        find_package(PythonLibs)
        include_directories ( SYSTEM
            ${PYTHON_INCLUDE_PATH}
            )
        swig_link_libraries(${TARGET} ${COFFEE_LINKING} ${PYTHON_LIBRARIES} )
    else()
        swig_link_libraries(${TARGET} ${COFFEE_LINKING} )
    endif()

    file(GLOB OUTPUT_FILES "${CMAKE_CURRENT_BINARY_DIR}/*.${OUTPUT_EXTENSION}")

#    install( FILES
#        "${SWIG_MODULE_${TARGET}_REAL_NAME}"
#        DESTINATION lib
#        )

    install(
        FILES
        ${OUTPUT_FILES}

        DESTINATION
        share/${OUTPUT_DEST}/
        )
endmacro()

# SWIG import
find_package(SWIG REQUIRED)

include(${SWIG_USE_FILE})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

include_directories (
    ${CMAKE_SOURCE_DIR}/include

    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}

    ${dirs}
    )

# We have interfaces in this directory
set(INTERFACE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/interfaces")

set(CFILE_DLANG "${INTERFACE_DIR}/coffeeswig_dlang.i")
set(CFILE_PYTHON2 "${INTERFACE_DIR}/coffeeswig_python2.i")
set(CFILE_PYTHON3 "${INTERFACE_DIR}/coffeeswig_python3.i")
set(CFILE_SHARP "${INTERFACE_DIR}/coffeeswig_sharp.i")

set(CMAKE_SWIG_FLAGS "")

link_directories( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${COFFEE_LIB_DIR})

coffee_gen_licenseinfo( CoffeeSwigShim "" )

coffee_gen_applicationinfo( CoffeeSwigShim
    "Coffee"
    "Coffee"
    1
    )

set ( CoffeeSwigShim_SRCS
    "coffeeswig.cpp;${LICENSE_FILE};${APPLICATION_INFO_FILE}"
    )

coffee_add_library(CoffeeSwigShim
    "${CoffeeSwigShim_SRCS}"
    "CoffeeCoreLoadable;CoffeeGLeam;CoffeeOpenAL;CoffeeBlam"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

# Link with some libraries
set(COFFEE_LINKING CoffeeSwigShim )

if(COFFEE_CSHARP)
    add_subdirectory(csharp)
endif()
if(COFFEE_DLANG)
    add_subdirectory(dlang)
endif()
if(COFFEE_PYTHON)
    add_subdirectory(python)
    add_subdirectory(python3)
endif()
