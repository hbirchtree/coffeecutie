add_definitions (-fvisibility=hidden)

if(LINUX_PARANOID_BUILD)
    add_definitions (
        -Winline
        -Wall
        -Werror
        -Wpadded
        )

    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions" )
endif()
