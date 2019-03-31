# Structure of source

 - `coffee`: engine source
 - `peripheral`: platform adaptations, eg. wrappers around `libc`, STL, POSIX and Win32
 - `ext`: extension libraries, weak bindings that might expose types from external libraries
 - `include`: global includes

## For typical subdirectories

 - `[base dir]/private`: compiled source, *.cpp and internal *.h
 - `[base dir]/include`: public headers for given subdirectory
 - `[base dir]/interface_include`: almost like headers, but are meant for headers including other headers. These are more useful for applications consuming the library than the library itself. For example:

            #include <lib/abc.h>
            #include <lib/def.h>
            #include <lib/ghi.h>
