# For Windows, we disable most annoying warnings to save compilation time. (damn it, Microsoft...)
# These include system headers (which are constantly giving off warnings)
#  as well as small warnings for padding and etc.
# Microsoft makes the worst headers of them all.
# We also set a target Windows NT version for some reason
# Vista is long gone, so 7 (0x0601) is a good target

add_definitions (
 -D_WIN32_WINNT=0x0601
 -D_CRT_SECURE_NO_WARNINGS
)

if(${CMAKE_CXX_COMPILER} MATCHES "/x86_amd64/")
    set ( CMAKE_LIBRARY_ARCHITECTURE "amd64" )
elseif(${CMAKE_CXX_COMPILER} MATCHES "/x86_arm/")
    set ( CMAKE_LIBRARY_ARCHITECTURE "arm" )
else()
    set ( CMAKE_LIBRARY_ARCHITECTURE "x86" )
endif()

message( "CMAKE_LIBRARY_ARCHITECTURE: ${CMAKE_LIBRARY_ARCHITECTURE}" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1" )
