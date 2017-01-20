Emscripten support
##################
Currently no support for SDL2, because SDL_GetVersion() is not visible (?)
Otherwise, the file API works in a temporary filesystem. The save API does not use IndexedDB, but it will at some point.
