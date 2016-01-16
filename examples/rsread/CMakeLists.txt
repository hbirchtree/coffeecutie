set ( RSReadTest_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_CORE_INCLUDE_DIR}
    )

coffee_add_example ( RSReadTest ${RSReadTest_SRCS} )

target_link_libraries ( RSReadTest
    CoffeeCore
    )