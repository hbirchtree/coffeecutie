find_package ( CEF REQUIRED )

coffee_add_library( CoffeeCEF private/ccef.cpp "${CEF_LIBRARIES};CoffeeImage" )

target_include_directories ( CoffeeCEF PUBLIC ${CEF_INCLUDE_DIR} )

coffee_add_application ( CCefSlave "CEFSlave" private/ccef_slave.cpp "${CEF_LIBRARIES}" )
