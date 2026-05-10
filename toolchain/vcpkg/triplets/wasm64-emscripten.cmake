set(VCPKG_ENV_PASSTHROUGH EMSDK PATH)

if(NOT DEFINED ENV{EMSDK})
   message(FATAL_ERROR "The EMSDK environment variable must be defined")
endif()

set(VCPKG_TARGET_ARCHITECTURE wasm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Emscripten)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Platform/Emscripten.cmake)
set(EMSCRIPTEN TRUE)
