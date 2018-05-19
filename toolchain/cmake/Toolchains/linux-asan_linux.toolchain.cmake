add_definitions (-fvisibility=hidden)

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -Wall -Wpedantic" )
