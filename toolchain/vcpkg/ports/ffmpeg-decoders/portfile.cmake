# LGPL: never linked statically into an application, whatever the triplet
# says (as for libwma)
set(VCPKG_LIBRARY_LINKAGE dynamic)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ffmpeg/ffmpeg
    REF "n${VERSION}"
    SHA512 c72f4062aecc16d8b2b1e8678d5efe3af4cfaa0cc7c0997052248f9e499e60c2463acf07877cf3b78b246ce3e8078cb043e8d97e90a6b50d06af32ff7369a788
    HEAD_REF master
)

# Decoders by port feature; a new codec is a feature plus a line here
set(DECODER_FEATURES
    "xma:xma1,xma2"
    "wma:wmav1,wmav2"
    "wmapro:wmapro"
    "vorbis:vorbis"
    "opus:opus"
    "mp3:mp3"
    "flac:flac"
    "adpcm:adpcm_ima_wav,adpcm_ms"
)
set(DECODERS "")
foreach(entry IN LISTS DECODER_FEATURES)
    string(REPLACE ":" ";" entry "${entry}")
    list(GET entry 0 feature)
    list(GET entry 1 decoders)
    if(feature IN_LIST FEATURES)
        list(APPEND DECODERS "${decoders}")
    endif()
endforeach()
if(NOT DECODERS)
    message(FATAL_ERROR "ffmpeg-decoders: pick at least one decoder feature")
endif()
list(JOIN DECODERS "," DECODERS)

vcpkg_cmake_get_vars(cmake_vars_file)
include("${cmake_vars_file}")

# ffmpeg's own build makes static archives of just those decoders; the
# wrapper project below links them into one shared library
set(FFMPEG_BUILD "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-ffmpeg")
set(FFMPEG_PREFIX "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-prefix")
file(REMOVE_RECURSE "${FFMPEG_BUILD}" "${FFMPEG_PREFIX}")
file(MAKE_DIRECTORY "${FFMPEG_BUILD}")

# Archives end up inside a shared library, so all of ffmpeg is PIC
set(PIC_FLAGS "")
if(NOT VCPKG_TARGET_IS_WINDOWS AND NOT VCPKG_TARGET_IS_MINGW)
    set(PIC_FLAGS "-fPIC")
endif()

set(OPTIONS
    --prefix=${FFMPEG_PREFIX}
    --enable-static
    --disable-shared
    --enable-pic
    --enable-cross-compile
    --disable-everything
    --disable-autodetect
    --disable-programs
    --disable-doc
    --disable-network
    --disable-avdevice
    --disable-avformat
    --disable-avfilter
    --disable-swscale
    --disable-swresample
    --disable-pthreads
    --disable-w32threads
    --disable-os2threads
    --disable-debug
    --enable-decoder=${DECODERS}
    "--cc=${VCPKG_DETECTED_CMAKE_C_COMPILER}"
    "--ar=${VCPKG_DETECTED_CMAKE_AR}"
    "--ranlib=${VCPKG_DETECTED_CMAKE_RANLIB}"
    "--nm=${VCPKG_DETECTED_CMAKE_NM}"
    "--extra-cflags=${VCPKG_COMBINED_C_FLAGS_RELEASE} ${PIC_FLAGS}"
    "--extra-ldflags=${VCPKG_COMBINED_SHARED_LINKER_FLAGS_RELEASE}"
)
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    list(APPEND OPTIONS --target-os=none --arch=x86_32 --disable-asm --disable-stripping)
elseif(VCPKG_TARGET_IS_OSX)
    list(APPEND OPTIONS --target-os=darwin --arch=${VCPKG_TARGET_ARCHITECTURE})
else()
    if(VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
        list(APPEND OPTIONS --arch=x86_64)
    elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64")
        list(APPEND OPTIONS --arch=aarch64)
    else()
        list(APPEND OPTIONS --arch=${VCPKG_TARGET_ARCHITECTURE})
    endif()
    if(VCPKG_TARGET_IS_ANDROID)
        list(APPEND OPTIONS --target-os=android)
    else()
        list(APPEND OPTIONS --target-os=linux)
    endif()
endif()

vcpkg_execute_required_process(
    COMMAND "${SOURCE_PATH}/configure" ${OPTIONS}
    WORKING_DIRECTORY "${FFMPEG_BUILD}"
    LOGNAME "configure-${TARGET_TRIPLET}"
)
vcpkg_execute_required_process(
    COMMAND make -j${VCPKG_CONCURRENCY} install-libs install-headers
    WORKING_DIRECTORY "${FFMPEG_BUILD}"
    LOGNAME "build-${TARGET_TRIPLET}"
)

set(EXTRA_OPTIONS "")
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    set(EXTRA_OPTIONS
        "-DCMAKE_SHARED_LINKER_FLAGS=-sSIDE_MODULE=1 -pthread"
        "-DCMAKE_PROJECT_INCLUDE=${CMAKE_CURRENT_LIST_DIR}/wasm-side-module.cmake"
    )
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/wrapper"
    OPTIONS
        "-DFFMPEG_PREFIX=${FFMPEG_PREFIX}"
        ${EXTRA_OPTIONS}
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME ffmpeg-decoders)

file(INSTALL "${FFMPEG_PREFIX}/include/" DESTINATION "${CURRENT_PACKAGES_DIR}/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/COPYING.LGPLv2.1")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
