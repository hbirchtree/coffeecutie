include_directories ( ${CMAKE_SOURCE_DIR}/bindings/swig/include )

coffee_application (
    TARGET SwigNativeTest
    TITLE "Native SWIG test"
    SOURCES main.cpp
    LIBRARIES CoffeeSwigShim CoffeeRenderer_Default CoffeeCore
    )
