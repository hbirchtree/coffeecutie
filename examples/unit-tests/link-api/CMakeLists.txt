coffee_add_elibrary ( EnvLibraryLoadTester SHARED libcode.cpp )

target_link_libraries(EnvLibraryLoadTester
    CoffeeCore
    )

coffee_add_example ( Unit_Link_Test01 test.cpp )

target_link_libraries ( Unit_Link_Test01
    CoffeeCore
    )