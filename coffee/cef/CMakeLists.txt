find_package ( CEF REQUIRED )

include_directories ( ${CEF_INCLUDE_DIR} )

coffee_add_library( CoffeeCEF private/ccef.cpp "${CEF_LIBRARIES};CoffeeImage" )

coffee_add_application ( CCefSlave "CEFSlave" private/ccef_slave.cpp "${CEF_LIBRARIES};CoffeeCore" )
