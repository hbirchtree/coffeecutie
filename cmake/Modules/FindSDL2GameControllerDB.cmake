set ( SDL2_GAMECONTROLLERDB
    "${CMAKE_SOURCE_DIR}/sample_data/SDL_GameControllerDB/"
    CACHE PATH "Path to SDL's gamecontrollerdb.txt" )

mark_as_advanced(SDL2_GAMECONTROLLERDB)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2GameControllerDB REQUIRED_VARS SDL2_GAMECONTROLLERDB)
