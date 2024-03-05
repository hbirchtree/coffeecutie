vcpkg_minimum_required(VERSION 2022-10-12) # for ${VERSION}
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO devkitPro/SDL
    REF d36a791c5207d3bd3e92b451dcddb97a010b73d4
    SHA512 d536efc7928551e7074d626c075ca89b39744acffe6c07390b5e7d918ca0220db6aab1f50436c1d5a16539e52d70d637fac1eb89c32e34407bc5555707acad1b
    HEAD_REF ogc-sdl-2.28
    PATCHES
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    ${configure_opts}
    OPTIONS ${FEATURE_OPTIONS}
        -DSDL_STATIC=ON
        -DSDL_SHARED=OFF
        -DSDL_FORCE_STATIC_VCRT=ON
        -DSDL_LIBC=ON
        -DSDL_TEST=OFF
        -DSDL_ARTS=OFF
        -DSDL_DISKAUDIO=OFF
        -DSDL_DUMMYAUDIO=OFF
        -DSDL_DUMMYVIDEO=OFF
        -DSDL_ESD=OFF
        -DSDL_FILE=ON
        -DSDL_FILESYSTEM=ON
        -DSDL_FUSIONSOUND=OFF
        -DSDL_RENDER=OFF
        -DSDL_THREADS=ON
        -DSDL_TIMERS=ON
    MAYBE_UNUSED_VARIABLES
        SDL_FORCE_STATIC_VCRT
)

vcpkg_cmake_install()

if(EXISTS "${CURRENT_PACKAGES_DIR}/cmake")
    vcpkg_cmake_config_fixup(CONFIG_PATH cmake)
elseif(EXISTS "${CURRENT_PACKAGES_DIR}/lib/cmake/SDL2")
    vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/SDL2)
elseif(EXISTS "${CURRENT_PACKAGES_DIR}/SDL2.framework/Resources")
    vcpkg_cmake_config_fixup(CONFIG_PATH SDL2.framework/Resources)
endif()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
    "${CURRENT_PACKAGES_DIR}/bin/sdl2-config"
    "${CURRENT_PACKAGES_DIR}/debug/bin/sdl2-config"
    "${CURRENT_PACKAGES_DIR}/SDL2.framework"
    "${CURRENT_PACKAGES_DIR}/debug/SDL2.framework"
)

file(GLOB BINS "${CURRENT_PACKAGES_DIR}/debug/bin/*" "${CURRENT_PACKAGES_DIR}/bin/*")
if(NOT BINS)
    file(REMOVE_RECURSE
        "${CURRENT_PACKAGES_DIR}/bin"
        "${CURRENT_PACKAGES_DIR}/debug/bin"
    )
endif()

set(DYLIB_COMPATIBILITY_VERSION_REGEX "set\\(DYLIB_COMPATIBILITY_VERSION (.+)\\)")
set(DYLIB_CURRENT_VERSION_REGEX "set\\(DYLIB_CURRENT_VERSION (.+)\\)")
file(STRINGS "${SOURCE_PATH}/CMakeLists.txt" DYLIB_COMPATIBILITY_VERSION REGEX ${DYLIB_COMPATIBILITY_VERSION_REGEX})
file(STRINGS "${SOURCE_PATH}/CMakeLists.txt" DYLIB_CURRENT_VERSION REGEX ${DYLIB_CURRENT_VERSION_REGEX})
string(REGEX REPLACE ${DYLIB_COMPATIBILITY_VERSION_REGEX} "\\1" DYLIB_COMPATIBILITY_VERSION "${DYLIB_COMPATIBILITY_VERSION}")
string(REGEX REPLACE ${DYLIB_CURRENT_VERSION_REGEX} "\\1" DYLIB_CURRENT_VERSION "${DYLIB_CURRENT_VERSION}")

if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL "debug")
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/debug/lib/pkgconfig/sdl2.pc" "-lSDL2main" "-lSDL2maind")
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/debug/lib/pkgconfig/sdl2.pc" "-lSDL2 " "-lSDL2d ")
endif()

vcpkg_fixup_pkgconfig()

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
