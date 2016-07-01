coffee_add_test ( Unit_File_Write01 "Cu_FileCpy" test01.cpp CoffeeCore )

coffee_add_test ( Unit_File_Mapping01 "Cu_FileMap" test02.cpp CoffeeCore )

# Testing anonymous buffers for memory allocation, can be more efficient that malloc() on some systems
coffee_add_test ( Unit_File_Scratch01 "Cu_FileScratchBuf" test03.cpp CoffeeCore )

if(NOT ANDROID)
    # The tests below require a 64-bit or 32-bit PAE system
    # These tests will not run on mobile devices, most likely (unless they have swap space, and tons of it)

    coffee_add_test ( Unit_File_Write02 "Cu_FileMassiveWrite" test04.cpp CoffeeCore )

    coffee_add_test ( Unit_File_Mapping02 "Cu_FileMassiveMap" test05.cpp CoffeeCore )
endif()
