set ( CBlamTesting_SRCS
    main.cpp
    )

include_directories(
    ${COFFEE_BLAM_INCLUDE_DIR}
    )

add_executable ( CBlamTesting ${CBlamTesting_SRCS} )

target_link_libraries ( CBlamTesting
    CoffeeCore
    CoffeeBlam
    )

target_enable_cxx11(CBlamTesting)
