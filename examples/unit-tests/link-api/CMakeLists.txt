coffee_add_elibrary ( EnvLibraryLoadTester SHARED libcode.cpp )

target_link_libraries(EnvLibraryLoadTester
    CoffeeCore
    )

coffee_add_test ( Unit_Link_Test01 "Cu_LinkingTest" test.cpp CoffeeCore )
