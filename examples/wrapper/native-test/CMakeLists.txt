include_directories ( ${CMAKE_SOURCE_DIR}/bindings/swig/include )

add_executable ( SwigNativeTest main.cpp )

target_link_libraries ( SwigNativeTest CoffeeSwigShim )
