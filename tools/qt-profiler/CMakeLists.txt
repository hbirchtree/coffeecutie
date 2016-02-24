include ( QtIncludes )

coffee_add_example ( CoffeeProfiler main.cpp )

target_link_libraries ( CoffeeProfiler CoffeeCore Qt5::Widgets )