vcpkg_download_distfile(ARCHIVE
    URLS https://github.com/powervr-graphics/Native_SDK/archive/9624ad3c8da0bbb92acba7cbe111e133b9682134.tar.gz
    FILENAME pvrtcdec.tar.gz
    SHA512 189136b314dd99c641ed4477783cf0b718fe88fe693098742c72666faf09e7f237f3969c18e0016d7e823ad39a931dd241a8f73b1b17ed98847f8c2ac2fd7121
)

vcpkg_extract_source_archive(
    SOURCE_PATH
    ARCHIVE ${ARCHIVE}
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${SOURCE_PATH})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/pvrtcdecConfig.cmake DESTINATION ${SOURCE_PATH})

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
)

vcpkg_cmake_install()

file(INSTALL ${SOURCE_PATH}/LICENSE.md DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
