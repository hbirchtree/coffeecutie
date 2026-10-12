# _INIT, so Platform/Emscripten.cmake can still add the triplet/port flags.
# -sMEMORY64 is deprecated and WASM_BIGINT is link-only; either one fails
# every -Werror feature check (openal-soft lost its symbol visibility).
set(CMAKE_C_FLAGS_INIT "-m64 -pthread")
set(CMAKE_CXX_FLAGS_INIT "-m64 -pthread")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-m64 -sWASM_BIGINT -pthread")

include("${CMAKE_CURRENT_LIST_DIR}/../Platform/Emscripten.cmake")
