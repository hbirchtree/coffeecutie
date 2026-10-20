cmake_minimum_required(VERSION 3.24)
project(ffmpeg-decoders C)

# libavcodec + libavutil (only the chosen decoders), whole, as one shared
# library; nothing else in it
set(FFMPEG_PREFIX "" CACHE PATH "Where the static ffmpeg build was installed")

add_library(ffmpeg-decoders SHARED stub.c)
set_target_properties(ffmpeg-decoders PROPERTIES VERSION 8.1.2 SOVERSION 8)
target_link_libraries(ffmpeg-decoders PRIVATE
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,${FFMPEG_PREFIX}/lib/libavcodec.a,${FFMPEG_PREFIX}/lib/libavutil.a>"
)
if(CMAKE_SYSTEM_NAME MATCHES "^(Linux|Android)$")
    # Only the public av* API; ff_* stay internal, which ffmpeg's x86
    # assembly needs to link into a shared library (as ffmpeg's own does)
    target_link_options(ffmpeg-decoders PRIVATE
        "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/exports.ver"
        -Wl,-Bsymbolic
    )
endif()
if(CMAKE_SYSTEM_NAME MATCHES "(MinGW|Windows)")
    target_link_libraries(ffmpeg-decoders PRIVATE bcrypt)
endif()
if(NOT EMSCRIPTEN)
    target_link_libraries(ffmpeg-decoders PRIVATE m)
endif()
target_include_directories(ffmpeg-decoders INTERFACE $<INSTALL_INTERFACE:include>)

install(TARGETS ffmpeg-decoders EXPORT ffmpeg-decoders-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(EXPORT ffmpeg-decoders-targets
    NAMESPACE ffmpeg-decoders::
    FILE ffmpeg-decoders-config.cmake
    DESTINATION share/ffmpeg-decoders
)
