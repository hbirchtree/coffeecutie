coffee_add_elibrary ( EnvLibraryLoadTester SHARED libcode.cpp "Core" )

add_definitions ( -DCMAKE_LINKING_DIRECTORY="${CMAKE_BINARY_DIR}/lib" )

coffee_add_test ( Unit_Link_Test01 "Cu_LinkingTest" test.cpp Core )
