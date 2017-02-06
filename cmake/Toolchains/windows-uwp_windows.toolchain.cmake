set ( CMAKE_SYSTEM_NAME "WindowsStore" )
set ( CMAKE_SYSTEM_VERSION "10.0" )

set ( WIN_UWP TRUE )

# For Windows, we disable most annoying warnings to save compilation time. (damn it, Microsoft...)
# These include system headers (which are constantly giving off warnings)
#  as well as small warnings for padding and etc.
# Microsoft makes the worst headers of them all.
# We also set a target Windows NT version for some reason
# Vista is long gone, so 7 (0x0601) is a good target

add_definitions (
 -D_WIN32_WINNT=0x0601
 -DWINUWP
 # We allow pure C string functions. Fuck, Microsoft.
 -D_CRT_SECURE_NO_WARNINGS
)

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1" )