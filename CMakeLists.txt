cmake_minimum_required( VERSION 3.1 )
cmake_policy ( SET CMP0048 NEW )

project( 
    Coffee 
    LANGUAGES C CXX
    VERSION 4.0
    )

include ( toolchain/cmake/Modules/SubprojectFunctions.cmake )

init_project()

set ( COFFEE_EXT_LIBRARY_DIR
    "${CMAKE_SOURCE_DIR}/src/libs"
    )

if(COFFEE_GENERATE_LIBRARIES)
    add_subdirectory( src )
endif()

if(COFFEE_GENERATE_LIBRARIES AND GENERATE_PROGRAMS)
    # Tools, includes the GUI profiler
    add_subdirectory(tools)

    # Add subdir for example applications
    if(GENERATE_EXAMPLES)
        add_subdirectory(examples)
    endif()
    if(GENERATE_TESTS)
        add_subdirectory(tests)
    endif()
endif()

# Add documentation directory, not built by default (increases build time)
add_subdirectory(toolchain/docs-gen)

end_project()
