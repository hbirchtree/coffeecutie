set ( CMAKE_SYSTEM_NAME "WindowsStore" )
set ( CMAKE_SYSTEM_VERSION "10.0" )

set ( WIN_UWP TRUE )

if(CMAKE_TOOLCHAIN_FILE)
endif()

# For Windows, we disable most annoying warnings to save compilation time. (damn it, Microsoft...)
# These include system headers (which are constantly giving off warnings)
#  as well as small warnings for padding and etc.
# Microsoft makes the worst headers of them all.
# We also set a target Windows NT version for some reason
# Vista is long gone, so 7 (0x0601) is a good target

add_definitions (
 # Windows UWP actually requires >= 0x0602, aka. Windows 8. We'll go for Windows 10, because UWP only runs on Windows 10
 -D_WIN32_WINNT=0x0A00 
 # We add this flag to help us a bit, a lot easier
 -DWINUWP
 # We allow pure C string functions. Fuck, Microsoft.
 -D_CRT_SECURE_NO_WARNINGS
 -D_SILENCE_CXX17_ALL_DEPRECATION_WARNINGS
)

include_directories ( "${NATIVE_LIBRARY_DIR}/include" )

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1 /std:c++17" CACHE STRING "" )
