find_package ( CEF REQUIRED )

message ("${CEF_INCLUDE_DIR} ${CEF_LIBRARIES}")

include_directories ( ${CEF_INCLUDE_DIR} )

coffee_add_library( CoffeeCEF private/ccef.cpp )
target_link_libraries ( CoffeeCEF
    ${CEF_LIBRARIES}
    CoffeeImage )

coffee_add_example ( CCefSlave "CEFSlave" private/ccef_slave.cpp "${CEF_LIBRARIES}" )
