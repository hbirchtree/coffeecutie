find_package ( CEF REQUIRED )

coffee_library (
    TARGET CoffeeCEF 
    
    SOURCES
    private/ccef.cpp
    
    LIBRARIES 
    ${CEF_LIBRARIES}
    CoffeeImage

    HEADER_DIRS
    $<BUILD_INTERFACE:${CEF_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

coffee_application (
    TARGET CCefSlave
   
    TITLE "CEFSlave"
    
    SOURCES 
    private/ccef_slave.cpp

    LIBRARIES 
    ${CEF_LIBRARIES}
    )
