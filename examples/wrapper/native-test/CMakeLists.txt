include_directories ( ${CMAKE_SOURCE_DIR}/bindings/swig/include )

coffee_add_application ( SwigNativeTest "Native SWIG test" main.cpp
    "CoffeeSwigShim;CoffeeRenderer_Default;CoffeeCore" )
