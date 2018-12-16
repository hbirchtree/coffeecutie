coffee_add_test ( Unit_File_API_Correctness
    "Cu_FileCorrectness" test09.cpp Core )

coffee_add_test ( Unit_File_Manipulate01
    "Cu_FileManipulate" test06.cpp Core )

coffee_add_test ( Unit_File_Write01 "Cu_FileCpy" test01.cpp Core )

coffee_add_test ( Unit_File_Mapping01 "Cu_FileMap" test02.cpp Core )

# Testing anonymous buffers for memory allocation
#  can be more efficient that malloc() on some systems
coffee_add_test ( Unit_File_Scratch01 "Cu_FileScratchBuf" test03.cpp Core )

if(NOT SKIP_HIGHMEM_TESTS)
    if(NOT ANDROID AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        # The tests below require a 64-bit or 32-bit PAE system
        # These tests will not run on mobile devices,
        #  most likely (unless they have swap space, and tons of it)
        # These tests might not fare well on Windows and <8GB RAM systems
        # TODO: Skip these tests on more low-memory systems

        coffee_add_test ( Unit_File_Write02
            "Cu_FileMassiveWrite" test04.cpp Core )

        coffee_add_test ( Unit_File_Mapping02
            "Cu_FileMassiveMap" test05.cpp Core )
    endif()
endif()

coffee_add_test ( Unit_Directory_List01
    "Cu_DirListing" test07.cpp Core )

if(NOT WIN32 AND NOT EMSCRIPTEN AND NOT BUILD_COVERAGE)
    coffee_add_test ( Unit_Symlink_Test01
        "Cu_Symlink" test08.cpp Core )
endif()

coffee_test (
    TARGET Unit_Url01
    TITLE "Cu_FileUrl"
    SOURCES test10.cpp
    LIBRARIES Core ASIO
    )

coffee_add_test ( Unit_Bytes01 "Cu_Bytes"
    test11.cpp Core
    )
