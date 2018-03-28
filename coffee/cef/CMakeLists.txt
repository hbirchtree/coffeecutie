find_package ( CEF REQUIRED )

coffee_add_library( CoffeeCEF private/ccef.cpp "${CEF_LIBRARIES};CoffeeImage" )

target_include_directories ( CoffeeCEF
    PUBLIC
    $<BUILD_INTERFACE:${CEF_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/>
    )

coffee_application (
    TARGET CCefSlave
    TITLE "CEFSlave"
    SOURCES private/ccef_slave.cpp
    LIBRARIES "${CEF_LIBRARIES}"
    )
