cmake_minimum_required(VERSION 3.1)

macro(CONFIG_SWIG_BUILD SFILE)
    # Explicit C++ build
    set_source_files_properties(${SFILE} PROPERTIES CPLUSPLUS ON)
    # Ignore missing headers
    set_property(SOURCE ${SFILE} PROPERTY SWIG_FLAGS "-importall" )
endmacro()

macro(SWIG_LANGUAGE_BINDING TARGET BIND CFILE OUTPUT_EXTENSION OUTPUT_DEST )
    CONFIG_SWIG_BUILD(${CFILE})

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

include_directories (
    ${CMAKE_SOURCE_DIR}/include

    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Define C++11 flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# We have interfaces in this directory
set(INTERFACE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/interfaces")

set(CFILE_DLANG "${INTERFACE_DIR}/coffeeswig_dlang.i")
set(CFILE_PYTHON "${INTERFACE_DIR}/coffeeswig_python.i")
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
    "CoffeeGLeam;CoffeeOpenAL;CoffeeBlam"
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
endif()
