coffee_library (
    TARGET MediaFfmpeg
    SOURCES
    private/interface.cpp
    LIBRARIES
    Core
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

dependency_get (
    REQUIRED
    SOURCE hbirchtree/native-library-bundle
    TAG ${NATIVE_LIB_BUNDLE_TAG}
    NAMES ffmpeg
    SIDELOAD "${ffmpeg_LOCAL_BUILD}"
    )

find_package ( PkgConfig REQUIRED )
set ( ENV{PKG_CONFIG_PATH} ${ffmpeg_DIR}/lib/pkgconfig )

pkg_check_modules (
    FFMPEG REQUIRED
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
    )

target_link_directories (
    MediaFfmpeg PUBLIC
    $<BUILD_INTERFACE:${ffmpeg_DIR}/lib>
    )
target_link_libraries (
    MediaFfmpeg PUBLIC
    $<BUILD_INTERFACE:${pkgcfg_lib_FFMPEG_avformat}>
    $<BUILD_INTERFACE:${pkgcfg_lib_FFMPEG_avcodec}>
    $<BUILD_INTERFACE:${pkgcfg_lib_FFMPEG_swresample}>
    $<BUILD_INTERFACE:${pkgcfg_lib_FFMPEG_swscale}>
    $<BUILD_INTERFACE:${pkgcfg_lib_FFMPEG_avutil}>
    lzma
    z
    )
target_include_directories (
    MediaFfmpeg PUBLIC
    $<BUILD_INTERFACE:${ffmpeg_DIR}/include>
    )
