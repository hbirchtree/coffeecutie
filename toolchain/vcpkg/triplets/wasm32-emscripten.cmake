set(VCPKG_ENV_PASSTHROUGH EMSDK PATH)

if(NOT DEFINED ENV{EMSDK})
   message(FATAL_ERROR "The EMSDK environment variable must be defined")
endif()

set(VCPKG_TARGET_ARCHITECTURE wasm32)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Emscripten)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Platform/Emscripten.cmake)
set(EMSCRIPTEN TRUE)

# examples/blam/cblam-testing (this triplet's only consumer) always links
# its Emscripten build with -pthread, unconditionally (EMSCRIPTEN_THREADPOOL_SIZE
# is set outside any feature guard) -- meaning every statically-linked
# vcpkg dependency ends up in a --shared-memory final binary regardless,
# which wasm-ld rejects for any object file not itself compiled with
# atomics/bulk-memory support ("--shared-memory is disallowed by X.o
# because it was not compiled with 'atomics' or 'bulk-memory' features").
# This was a latent gap, not a new requirement: nothing pulled in for this
# triplet before had real atomics-touching code (std::atomic, threading)
# to trip it, until gamenetworkingsockets' webrtc-datachannel feature
# (and its transitive protobuf/abseil dependencies) did -- found by
# actually attempting the link, not anticipated upfront. Setting it here
# once, for the whole triplet, avoids patching -pthread into every
# affected port's own build individually.
set(VCPKG_C_FLAGS "-pthread")
set(VCPKG_CXX_FLAGS "-pthread")
