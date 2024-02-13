set(LIBS)
if(NOT EMSCRIPTEN)
    option(BUILD_OPENSSL "Build ASIO with OpenSSL support" ON)

    # OpenSSL used for ASIO networking
    if(BUILD_OPENSSL)
      set(OPENSSL_ROOT_DIR "${OpenSSL_DIR}")
      set(ENV{OPENSSL_ROOT_DIR} "${OpenSSL_DIR}")

      find_package(OpenSSL REQUIRED)
    endif()

    set(LIBS OpenSSL::SSL)
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
endif()

coffee_library(
  TARGET ASIO
  SOURCES private/asio_worker.cpp private/net_resource.cpp
          private/net_profiling.cpp
  LIBRARIES Core CoreState ${LIBS}
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


if(NOT EMSCRIPTEN)
    target_compile_definitions(ASIO PUBLIC -DASIO_STANDALONE -DASIO_USE_SSL)
else()
    target_compile_definitions(ASIO PUBLIC -DUSE_EMSCRIPTEN_HTTP)
    target_compile_options(ASIO PUBLIC -sFETCH)
    target_link_options(ASIO PUBLIC -sFETCH)
endif()
