set ( ANDROID_ABI "x86" CACHE STRING "" )
set ( ANDROID_NATIVE_API_LEVEL 12 CACHE STRING "" )

# Drop shared builds, they're garbage
set ( SDL_STATIC        ON  CACHE BOOL "" )
set ( SDL_STATIC_PIC    ON  CACHE BOOL "" )
set ( SDL_SHARED        OFF CACHE BOOL "" )

set ( SDL_ATOMIC        OFF CACHE BOOL "" )
set ( SDL_AUDIO         OFF CACHE BOOL "" )
set ( SDL_CPUINFO       OFF CACHE BOOL "" )
set ( SDL_FILE          OFF CACHE BOOL "" )
set ( SDL_THREADS       OFF CACHE BOOL "" )

set ( DIRECTX           OFF CACHE BOOL "" )
