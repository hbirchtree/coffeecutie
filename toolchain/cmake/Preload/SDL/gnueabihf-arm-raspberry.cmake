# Drop shared builds, they're garbage
set ( SDL_STATIC        ON  CACHE BOOL "" )
set ( SDL_STATIC_PIC    ON  CACHE BOOL "" )
set ( SDL_SHARED        OFF CACHE BOOL "" )

set ( SDL_ATOMIC        OFF CACHE BOOL "" )
set ( SDL_AUDIO         OFF CACHE BOOL "" )
set ( SDL_CPUINFO       OFF CACHE BOOL "" )
set ( SDL_FILE          OFF CACHE BOOL "" )
set ( SDL_THREADS       OFF CACHE BOOL "" )

set ( ESD               OFF CACHE BOOL "" )
set ( OSS               OFF CACHE BOOL "" )
set ( ESD_SHARED        OFF CACHE BOOL "" )
set ( FUSIONSOUND       OFF CACHE BOOL "" )

set ( VIDEO_X11         OFF CACHE BOOL "" )
set ( VIDEO_OPENGL      OFF CACHE BOOL "" )
set ( VIDEO_VIVANTE     OFF CACHE BOOL "" )
set ( VIDEO_MIR         OFF CACHE BOOL "" )
