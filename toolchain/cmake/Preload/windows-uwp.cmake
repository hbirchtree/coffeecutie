set ( BUILD_ASIO    OFF CACHE BOOL "" )
set ( BUILD_AUDIO   OFF CACHE BOOL "" )
set ( BUILD_GLES    ON  CACHE BOOL "" )

set ( SDL2_MAIN_C_FILE "${NATIVE_LIBRARY_DIR}/src/SDL_winrt_main_NonXAML.cpp"
    CACHE FILEPATH ""
    )
