set ( CMAKE_SYSTEM_NAME Generic )
set ( CMAKE_SYSTEM_PROCESSOR powerpc-eabi )
set ( CMAKE_CROSSCOMPILING 1 )
set ( TOOLCHAIN_PREFIX "powerpc-eabi" )

set ( CMAKE_EXECUTABLE_SUFFIX "elf" )

set ( GAMECUBE TRUE )

set ( CMAKE_CXX_COMPILE_FEATURES cxx_constexpr )

set ( CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" )
set ( CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}-g++" )

find_program ( ELF2DOL elf2dol )

set ( ELF2DOL "${ELF2DOL}" CACHE STRING "" )

set ( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -mogc -mcpu=750 -meabi -mhard-float -I$ENV{PPCPORTLIBS_CUBE}/include" CACHE STRING "" )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mogc -mcpu=750 -meabi -mhard-float -std=c++11 -I$ENV{PPCPORTLIBS_CUBE}/include" CACHE STRING "" )

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

set ( CMAKE_FIND_ROOT_PATH 
    $ENV{PPCPORTLIBS_CUBE}
    ${NATIVE_LIBRARY_DIR}
    ${COFFEE_ROOT_DIR}
    ${CMAKE_SOURCE_DIR}/src/libs
    )
set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

set ( CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -I$ENV{PPCPORTLIBS_CUBE}/include -L$ENV{PPCPORTLIBS_CUBE}/lib -logc -lm"
    CACHE STRING ""
    )

set ( CMAKE_LIBRARY_ARCHITECTURE "powerpc-eabi" CACHE STRING "" )

set ( GAMECUBE ON CACHE BOOL "" )

add_definitions ( -DGEKKO -D__GEKKO__ )
