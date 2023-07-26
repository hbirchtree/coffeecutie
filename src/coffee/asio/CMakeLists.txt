option(BUILD_OPENSSL "Build ASIO with OpenSSL support" ON)

# OpenSSL used for ASIO networking
if(BUILD_OPENSSL)
  set(OPENSSL_ROOT_DIR "${OpenSSL_DIR}")
  set(ENV{OPENSSL_ROOT_DIR} "${OpenSSL_DIR}")

  find_package(OpenSSL REQUIRED)
endif()

if(TARGET OpenSSL::SSL)
  set(LIBS OpenSSL::SSL)
  if(NOT WIN32
     AND NOT MINGW
     AND NOT MSYS
     AND NOT EMSCRIPTEN
  )
    list(APPEND LIBS dl)
  endif()
else()
  set(LIBS $<BUILD_INTERFACE:${OPENSSL_LIBRARIES}>)
  if(NOT ANDROID)
    set(LIBS ${LIBS} Threads::Threads)
  endif()
endif()

find_package(Threads QUIET)

if(TARGET Threads::Threads AND NOT ANDROID)
  set(LIBS ${LIBS} Threads::Threads)
endif()

coffee_library(
  TARGET ASIO
  SOURCES private/asio_worker.cpp private/net_resource.cpp
          private/net_profiling.cpp
  LIBRARIES Core CoreState ${LIBS}
  HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

#target_precompile_headers(
#  ASIO
#  PUBLIC
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/http.h>
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/net_resource.h>
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/tcp_socket.h>
#  $<$<COMPILE_LANGUAGE:CXX>:coffee/asio/udp_socket.h>
#)

set(ASIO_DIR "${COFFEE_EXT_LIBRARY_DIR}/asio")

header_install(ASIO $<BUILD_INTERFACE:${ASIO_DIR}/asio/include> "")

target_include_directories(
  ASIO PUBLIC $<BUILD_INTERFACE:${ASIO_DIR}/asio/include>
)

if(BUILD_OPENSSL)
  if(WIN32)
    target_link_libraries(ASIO PUBLIC crypt32 # Necessary for OpenSSL
    )
  endif()

  target_include_directories(
    ASIO SYSTEM PUBLIC $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>
  )
  target_compile_definitions(ASIO PUBLIC -DCOFFEE_ENABLE_SSL -DASIO_USE_SSL)
endif()

target_compile_definitions(ASIO PUBLIC -DASIO_STANDALONE)
