find_package ( CEF REQUIRED )

include_directories ( ${CEF_INCLUDE_DIR} )

coffee_add_library( CoffeeCEF private/ccef.cpp )
target_link_libraries ( CoffeeCEF
    CoffeeImage
    ${CEF_LIBRARIES} )

coffee_add_example ( CoffeeCefSlave
    private/ccef_slave.cpp
    )

target_link_libraries ( CoffeeCefSlave
    ${CEF_LIBRARIES}
    )