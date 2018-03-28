# Tests what happens to subdirectories and files on different systems, and whether it is desired behavior
# This comes in handy to detect bugs, you know

coffee_application (
    TARGET Unit_Packaging_Test01
    TITLE "Cu_PackagingTest"

    SOURCES test01.cpp

    LIBRARIES CoffeeCore CoffeeTesting
    RESOURCES "${COFFEE_ASSET_DIRECTORY}/packaging-test"
    )
