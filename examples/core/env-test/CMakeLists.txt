set ( EnvTest_SRCS
    main.cpp
    )

coffee_add_example ( EnvTest ${EnvTest_SRCS} )

include_directories(
    ${COFFEE_GRAPHICS_OPENGL_INCLUDE_DIR}
    )

target_link_libraries ( EnvTest
    CoffeeCore
    CoffeeImage
    )

#Testing an external library
set ( EnvLibraryLoadTester_SRCS
    libcode.cpp
    )

coffee_add_elibrary ( EnvLibraryLoadTester SHARED "${EnvLibraryLoadTester_SRCS}" )

target_link_libraries(EnvLibraryLoadTester
    CoffeeCore
    )
