# Tests what happens to subdirectories and files on different systems, and whether it is desired behavior
# This comes in handy to detect bugs, you know

coffee_add_example_long (
    Unit_Packaging_Test01 "Cu_PackagingTest"

    test01.cpp

    CoffeeCore ""
    "${COFFEE_ASSET_DIRECTORY}/packaging-test" )
