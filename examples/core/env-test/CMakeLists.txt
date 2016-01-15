set ( EnvTest_SRCS
    main.cpp
    )

add_executable ( EnvTest ${EnvTest_SRCS} )

include_directories(
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    )

target_link_libraries ( EnvTest
    CoffeeCore
    )

target_enable_cxx11(EnvTest)

#Testing an external library
set ( EnvLibraryLoadTester_SRCS
    libcode.cpp
    )

add_library( EnvLibraryLoadTester SHARED ${EnvLibraryLoadTester_SRCS} )

target_link_libraries(EnvLibraryLoadTester
    CoffeeCore
    )

target_enable_cxx11(EnvLibraryLoadTester)
