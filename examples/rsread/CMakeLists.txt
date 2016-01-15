set ( RSReadTest_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_CORE_INCLUDE_DIR}
    )

add_executable ( RSReadTest ${RSReadTest_SRCS} )

target_link_libraries ( RSReadTest
    CoffeeCore
    )

target_enable_cxx11(RSReadTest)
