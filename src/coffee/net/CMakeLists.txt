set(LIBS)
if(NOT EMSCRIPTEN)
    option(BUILD_OPENSSL "Build Net with OpenSSL support" ON)
    if(BUILD_OPENSSL AND MINGW)
      # Version used in MinGW is older, because newer versions
      # from vcpkg refuse to build on the MinGW toolchain used :(
      find_package(OpenSSL REQUIRED)
      set(LIBS ${OPENSSL_LIBRARIES})
    elseif(BUILD_OPENSSL)
      set(OPENSSL_ROOT_DIR "${OpenSSL_DIR}")
      set(ENV{OPENSSL_ROOT_DIR} "${OpenSSL_DIR}")

      find_package(OpenSSL CONFIG REQUIRED)
      set(LIBS OpenSSL::SSL)
    endif()

    if(NOT WIN32
     AND NOT MINGW
     AND NOT MSYS
     AND NOT EMSCRIPTEN
    )
        list(APPEND LIBS dl)
    endif()

    find_package(Threads QUIET)

    if(TARGET Threads::Threads AND NOT ANDROID AND NOT WIN32)
      set(LIBS ${LIBS} Threads::Threads)
    endif()

    find_package(CURL REQUIRED)
    set(LIBS ${LIBS} CURL::libcurl)
endif()

coffee_library(
  TARGET Net
  SOURCES
    private/curl_context.cpp
    private/net_resource.cpp
    private/net_profiling.cpp
  LIBRARIES Core CoreState ${LIBS}
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(NOT EMSCRIPTEN)
    target_compile_definitions(Net PUBLIC -DUSE_CURL)
else()
    target_compile_definitions(Net PUBLIC -DUSE_EMSCRIPTEN_HTTP)
    target_compile_options(Net PRIVATE -sFETCH)
    target_link_options(Net PUBLIC -sFETCH)
endif()
